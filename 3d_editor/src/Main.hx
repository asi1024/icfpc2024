import js.Browser;
import muun.la.Vec2;
import pot.core.App;
import pot.graphics.bitmap.BitmapGraphics;
import pot.input.KeyValue;

using StringTools;

inline final SIZE_SHIFT = 5; // 32x32
inline final SIZE = 1 << SIZE_SHIFT;
inline final SIZE_MASK = SIZE - 1;
inline final SIZE2 = SIZE * SIZE;

enum abstract Op(Int) {
	var Add;
	var Sub;
	var Mult;
	var Div;
	var Mod;
	var Eq;
	var Neq;
	var Warp;
	var A;
	var B;
	var S;
}

@:using(Main.CellDataTools)
enum CellData {
	None;
	Int(a:BigInt);
	Arrow(di:Int, dj:Int);
	Op(op:Op);
}

private class CellDataTools {
	public static function toString(data:CellData):String {
		return switch data {
			case None:
				"";
			case Int(a):
				a.toString();
			case Arrow(di, dj):
				switch [di, dj] {
					case [-1, 0]:
						"^";
					case [1, 0]:
						"v";
					case [0, -1]:
						"<";
					case [0, 1]:
						">";
					case _:
						"ERR";
				}
			case Op(op):
				switch op {
					case Add:
						"+";
					case Sub:
						"-";
					case Mult:
						"*";
					case Div:
						"/";
					case Mod:
						"%";
					case Eq:
						"=";
					case Neq:
						"≠";
					case Warp:
						"@";
					case A:
						"A";
					case B:
						"B";
					case S:
						"S";
				}
		}
	}
}

class Cell {
	public final i:Int;
	public final j:Int;
	public var data:CellData;
	public var text:String = "";
	public var selected:Bool = false;
	public var next:CellData = null;
	public var willBeRemoved:Bool = false;

	public function new(i:Int, j:Int, data:CellData = None) {
		this.i = i;
		this.j = j;
		this.data = data;
	}

	public function copy():Cell {
		return new Cell(i, j, data);
	}

	public static function parseData(s:String):CellData {
		return switch s {
			case "":
				None;
			case "A":
				Op(A);
			case "B":
				Op(B);
			case "S":
				Op(S);
			case "+":
				Op(Add);
			case "-":
				Op(Sub);
			case "*":
				Op(Mult);
			case "/":
				Op(Div);
			case "%":
				Op(Mod);
			case "@":
				Op(Warp);
			case "=":
				Op(Eq);
			case "≠":
				Op(Neq);
			case "<":
				Arrow(0, -1);
			case ">":
				Arrow(0, 1);
			case "^":
				Arrow(-1, 0);
			case "v":
				Arrow(1, 0);
			case _:
				final num = Std.parseInt(s);
				num != null ? Int(num) : None;
		}
	}
}

class Field {
	public final cells:Array<Cell> = [];
	public final t:Int;

	public function new(t:Int = 0) {
		this.t = t;
		for (i in 0...SIZE) {
			for (j in 0...SIZE) {
				cells.push(new Cell(i, j));
			}
		}
	}

	public function toString():String {
		var mins = [SIZE, SIZE];
		var maxs = [-1, -1];
		for (c in cells) {
			if (c.data != None) {
				mins[0] = min(mins[0], c.i);
				mins[1] = min(mins[1], c.j);
				maxs[0] = max(maxs[0], c.i);
				maxs[1] = max(maxs[1], c.j);
			}
		}
		if (maxs[0] == -1)
			return ".";
		var res = "";
		for (i in mins[0]...maxs[0] + 1) {
			var line = "";
			for (j in mins[1]...maxs[1] + 1) {
				final data = cellAt(i, j).data;
				line += " " + (data == None ? "." : data.toString());
			}
			res += line.trim() + "\n";
		}
		return res.trim();
	}

	public function fromString(data:String):Void {
		for (c in cells) {
			c.data = None;
		}
		final lines = data.trim().split("\n").map(l -> ~/ +/g.replace(l.trim(), " ").split(" "));
		final w = lines[0].length;
		final h = lines.length;
		final di = SIZE - h >> 1;
		final dj = SIZE - w >> 1;
		trace(SIZE + " " + w + " " + h);
		for (i => line in lines) {
			for (j => s in line) {
				cellAt(i + di, j + dj).data = Cell.parseData(s);
			}
		}
	}

	public function cellAt(i:Int, j:Int):Cell {
		return cells[(i & SIZE_MASK) << SIZE_SHIFT | (j & SIZE_MASK)];
	}

	public function inBounds(i:Int, j:Int):Bool {
		return i == i & SIZE_MASK && j == j & SIZE_MASK;
	}

	public function copy(dt:Int = 0):Field {
		final res = new Field(t + dt);
		for (i in 0...SIZE2) {
			res.cells[i] = cells[i].copy();
		}
		return res;
	}
}

class World {
	public final input:Field = new Field();
	public final ticks:Array<Field> = [];
	public final times:Array<Field> = [];
	public var failReason:String = "";
	public var result:CellData = None;

	public function new() {
	}

	public function run(a:BigInt, b:BigInt):Void {
		ticks.clear();
		times.clear();
		failReason = "";
		result = None;
		var f:Field = input.copy();
		for (c in f.cells) {
			if (c.data.match(Op(A))) {
				c.data = Int(a);
			}
			if (c.data.match(Op(B))) {
				c.data = Int(b);
			}
		}
		final maxSteps = 10000;
		function fail(msg:String, c:Cell = null):Void {
			failReason += msg + "\n";
			if (c != null) {
				ticks[ticks.length - 1].cellAt(c.i, c.j).selected = true;
			}
		}
		var nothingHappenedCount = 0;
		for (tick in 0...maxSteps) {
			ticks.push(f);
			times.push(f);

			if (result != None)
				break;

			f = f.copy(1);
			var nothingHappened = true;

			inline function writeTo(c:Cell, data:CellData):Void {
				nothingHappened = false;
				if (c.next != null) {
					fail('written twice at (${c.i}, ${c.j})', c);
				}
				c.next = data;
				if (c.data.match(Op(S))) {
					c.selected = true;
					if (result != None && !result.equals(data)) {
						fail('different results written: ${result.toString()} and ${data.toString()}', c);
					} else {
						result = data;
					}
				}
			}

			final warpTargets:Array<{
				i:Int,
				j:Int,
				t:Int,
				data:CellData
			}> = [];

			for (c in f.cells) {
				switch c.data {
					case None:
					case Int(a):
					case Arrow(di, dj):
						final src = f.cellAt(c.i - di, c.j - dj);
						final dst = f.cellAt(c.i + di, c.j + dj);
						if (src.data != None) {
							writeTo(dst, src.data);
							src.willBeRemoved = true;
						}
					case Op(op):
						final l = f.cellAt(c.i, c.j - 1);
						final t = f.cellAt(c.i - 1, c.j);
						final b = f.cellAt(c.i + 1, c.j);
						final r = f.cellAt(c.i, c.j + 1);
						function binOp(f:(x:BigInt, y:BigInt) -> BigInt):Void {
							switch [l.data, t.data] {
								case [Int(x), Int(y)]:
									l.willBeRemoved = true;
									t.willBeRemoved = true;
									final res = f(x, y);
									writeTo(b, Int(res));
									writeTo(r, Int(res));
								case _:
							}
						}
						function binOpPass(f:(x:CellData, y:CellData) -> Bool):Void {
							if (l.data != None && t.data != None && f(l.data, t.data)) {
								l.willBeRemoved = true;
								t.willBeRemoved = true;
								writeTo(b, l.data);
								writeTo(r, t.data);
							}
						}
						switch op {
							case Add:
								binOp((x, y) -> x + y);
							case Sub:
								binOp((x, y) -> x - y);
							case Mult:
								binOp((x, y) -> x * y);
							case Div:
								binOp((x, y) -> y == 0 ? {
									fail("zero division", c);
									0;
								} : x / y);
							case Mod:
								binOp((x, y) -> y == 0 ? {
									fail("zero division (mod)", c);
									0;
								} : x % y);
							case Eq | Neq:
								binOpPass((x, y) -> (switch [x, y] {
									case [Int(x), Int(y)] if (x == y):
										true;
									case [Op(x), Op(y)] if (x == y):
										true;
									case _:
										false;
								}) == (op == Eq));
							case Warp:
								switch [t.data, l.data, r.data, b.data] {
									case [v, Int(dx), Int(dy), Int(dt)] if (v != None):
										do {
											if (dt <= 0) {
												fail("dt must be positive", c);
												break;
											}
											final x = c.j - dx;
											final y = c.i - dy;
											if (x < 0 || x >= SIZE) {
												fail("x out of bounds: " + x.toString(), c);
												break;
											}
											if (y < 0 || y >= SIZE) {
												fail("y out of bounds: " + y.toString(), c);
												break;
											}
											final t = (f.t - 1) - dt; // -1 because time alrealy advanced
											if (t < 0) {
												fail("try to warp to negative time: " + t.toString(), c);
												break;
											}
											warpTargets.push({
												i: y.toInt(),
												j: x.toInt(),
												t: t.toInt(),
												data: v
											});
											nothingHappened = false;
										} while (false);
									case _:
								}
							case A | B | S:
								// do nothing
						}
				}
			}
			for (w1 in warpTargets) {
				for (w2 in warpTargets) {
					if (w1 == w2)
						break;
					if (w1.t != w2.t) {
						fail('tried to warp to different times: ${w1.t} and ${w2.t}');
						break;
					}
					if (w1.i == w2.i && w1.j == w2.j && !w1.data.equals(w2.data)) {
						fail('tried to write different data at (${w1.i}, ${w1.j}, t=${w1.t}): ${w1.data.toString()} and ${w2.data.toString()}',
							f.cellAt(w1.i, w1.j));
						break;
					}
				}
			}
			if (failReason != "")
				break;
			if (nothingHappened)
				if (++nothingHappenedCount >= 10)
					break;
			if (warpTargets.length > 0 && result == None) {
				final t = warpTargets[0].t;
				while (times.length > t + 1) {
					times.pop();
				}
				f = times[t].copy();
				for (w in warpTargets) {
					final c = f.cellAt(w.i, w.j);
					c.data = w.data;
					c.selected = true;
				}
				times.pop();
			} else {
				for (c in f.cells) {
					if (c.willBeRemoved) {
						c.data = None;
					}
					if (c.next != null) {
						c.data = c.next;
					}
				}
			}
		}
	}
}

enum Mode {
	Edit;
	Input(index:Int);
	Paste;
	Write(cell:Cell);
	View;
}

class Main extends App {
	var g:BitmapGraphics;
	final w:World = new World();
	final center:Vec2 = Vec2.one * (SIZE * 0.5);
	var scale:Float = 32;
	final panPos:Vec2 = Vec2.zero;
	var panning:Bool = false;
	var mode:Mode = Edit;
	var textInput:String = "";
	var selecting:Bool = true;
	var dragging:Bool = false;
	final copiedCells:Array<Cell> = [];
	var playing:Bool = false;
	var tick:Int = 0;
	var cursorI:Int = SIZE >> 1;
	var cursorJ:Int = SIZE >> 1;
	final inputs:Array<BigInt> = [1, 1];

	override function setup():Void {
		input.scalingMode = Canvas;
		g = new BitmapGraphics(canvas.getContext2d());

		Browser.document.getElementById("import").onclick = () -> {
			Browser.window.navigator.clipboard.readText().then(text -> {
				w.input.fromString(text);
				mode = Edit;
			});
		}
		Browser.document.getElementById("export").onclick = () -> {
			Browser.window.navigator.clipboard.writeText(w.input.toString());
		}

		pot.start();
	}

	override function update():Void {
		final mouse = input.mouse;
		final wpos = toWorld(mouse.pos);
		if (mouse.dright == 1) {
			panPos <<= wpos;
		}
		if (mouse.dright == -1)
			panning = false;
		if (mouse.right) {
			final wdiff = panPos - wpos;
			center += wdiff;
		}
		if (mouse.wheelY != 0) {
			scale *= Math.exp(-mouse.wheelY * 0.005);
			scale = clamp(scale, 4, 128);
			final wpos2 = toWorld(input.mouse.pos);
			center += wpos - wpos2;
		}
		center <<= center.clamp(Vec2.one * 0, Vec2.one * SIZE);
		processInput();
	}

	extern inline function toWorld(screen:Vec2):Vec2 {
		return ((screen - pot.size * 0.5) / scale) + center;
	}

	extern inline function toScreen(world:Vec2):Vec2 {
		return (world - center) * scale + pot.size * 0.5;
	}

	extern inline function cellAt(field:Field, pos:Vec2):Null<Cell> {
		final wpos = toWorld(pos);
		final j = Math.floor(wpos.x);
		final i = Math.floor(wpos.y);
		if (i < 0 || i >= SIZE || j < 0 || j >= SIZE)
			return null;
		return field.cellAt(i, j);
	}

	function key(keyValue:String):Bool {
		return input.keyboard.isKeyDown(keyValue) || input.keyboard.isKeyDown(keyValue.toUpperCase());
	}

	function editText():Void {
		if (mode.match(Input(_))) {
			input.keyboard.forEachDownKey(c -> {
				switch c {
					case KeyValue.Backspace:
						textInput = textInput.substr(0, textInput.length - 1);
					case "-":
						textInput = "-";
					case dig if (KeyValue.DIGITS.contains(dig)):
						textInput += dig;
						var bi:BigInt;
						try {
							bi = textInput;
						} catch (e) {
							textInput = dig;
							bi = textInput;
						}
						textInput = bi.toString();
				}
			});
		} else {
			input.keyboard.forEachDownKey(c -> {
				switch c {
					case KeyValue.Backspace:
						textInput = textInput.substr(0, textInput.length - 1);
					case "a":
						textInput = "A";
					case "b":
						textInput = "B";
					case "s":
						textInput = "S";
					case "+":
						textInput = "+";
					case "-":
						textInput = "-";
					case "*":
						textInput = "*";
					case "/":
						textInput = "/";
					case "%":
						textInput = "%";
					case "@":
						textInput = "@";
					case "=":
						textInput = "=";
					case "!" | "#":
						textInput = "≠";
					case "^" | "k" | KeyValue.ArrowUp:
						textInput = "^";
					case "v" | "j" | KeyValue.ArrowDown:
						textInput = "v";
					case "<" | "," | "h" | KeyValue.ArrowLeft:
						textInput = "<";
					case ">" | "." | "l" | KeyValue.ArrowRight:
						textInput = ">";
					case dig if (KeyValue.DIGITS.contains(dig)):
						textInput += dig;
						if (Std.parseInt(textInput) == null)
							textInput = dig;
						textInput = Std.string(clamp(Std.parseInt(textInput), -99, 99));
				}
			});
		}
	}

	function clearSelection(f:Field):Void {
		for (c in f.cells) {
			c.selected = false;
		}
	}

	function deleteSelection(f:Field):Void {
		for (c in f.cells) {
			if (c.selected) {
				c.data = None;
				c.selected = false;
			}
		}
	}

	function pasteAt(f:Field, i:Int, j:Int):Void {
		clearSelection(f);
		for (src in copiedCells) {
			final ii = i + src.i;
			final jj = j + src.j;
			if (f.inBounds(ii, jj)) {
				final dst = f.cellAt(ii, jj);
				dst.data = src.data;
				dst.selected = true;
			}
		}
	}

	function selectedCells(f:Field):Array<Cell> {
		return f.cells.filter(c -> c.selected);
	}

	function runSimulation():Void {
		w.run(inputs[0], inputs[1]);
	}

	function processInput():Void {
		final mouse = input.mouse;
		final kb = input.keyboard;
		final f = w.input;
		switch mode {
			case Edit:
				inline function editCell(c:Cell):Void {
					mode = Write(c);
					textInput = c.data.toString();
					dragging = false;
				}

				do {
					final cell = cellAt(f, mouse.pos);
					if (cell != null) {
						if (mouse.dleft == 1) {
							cursorI = cell.i;
							cursorJ = cell.j;
							dragging = true;
							if (selectedCells(f).length == 1 && cell.selected) {
								editCell(cell);
								break;
							}
							selecting = !cell.selected;
							if (!kb.isShiftDown() && !cell.selected) {
								clearSelection(f);
							}
						}
						if (!mouse.left)
							dragging = false;
						if (mouse.left && dragging) {
							cursorI = cell.i;
							cursorJ = cell.j;
							cell.selected = selecting;
						}

						final cellToEdit = if (key("e")) {
							f.cellAt(cursorI, cursorJ);
						} else {
							null;
						}
						if (cellToEdit != null) {
							editCell(cellToEdit);
							break;
						}

						final cut = key("x");
						if (key("c") || cut) {
							final sel = selectedCells(f);
							if (!sel.empty()) {
								copiedCells.clear();
								var mins = [SIZE, SIZE];
								var maxs = [0, 0];
								for (c in sel) {
									mins[0] = min(mins[0], c.i);
									mins[1] = min(mins[1], c.j);
									maxs[0] = max(maxs[0], c.i);
									maxs[1] = max(maxs[1], c.j);
								}
								final mi = mins[0] + maxs[0] >> 1;
								final mj = mins[1] + maxs[1] >> 1;
								for (c in sel) {
									final di = c.i - mi;
									final dj = c.j - mj;
									final data = c.data;
									copiedCells.push(new Cell(di, dj, data));
									if (cut) {
										c.selected = false;
										c.data = None;
									}
								}
								break;
							}
						}
					}
					if (key("w") || key("k") || key(ArrowUp)) {
						cursorI = clamp(cursorI - 1, 0, SIZE_MASK);
						clearSelection(f);
						f.cellAt(cursorI, cursorJ).selected = true;
						break;
					}
					if (key("s") || key("j") || key(ArrowDown)) {
						cursorI = clamp(cursorI + 1, 0, SIZE_MASK);
						clearSelection(f);
						f.cellAt(cursorI, cursorJ).selected = true;
						break;
					}
					if (key("a") || key("h") || key(ArrowLeft)) {
						cursorJ = clamp(cursorJ - 1, 0, SIZE_MASK);
						clearSelection(f);
						f.cellAt(cursorI, cursorJ).selected = true;
						break;
					}
					if (key("d") || key("l") || key(ArrowRight)) {
						cursorJ = clamp(cursorJ + 1, 0, SIZE_MASK);
						clearSelection(f);
						f.cellAt(cursorI, cursorJ).selected = true;
						break;
					}
					if (key("1")) {
						textInput = "0";
						mode = Input(0);
						break;
					}
					if (key("2")) {
						textInput = "0";
						mode = Input(1);
						break;
					}
					if (key(Escape)) {
						clearSelection(f);
						break;
					}
					if (key(Delete)) {
						deleteSelection(f);
						break;
					}
					if (key("v") && !copiedCells.empty()) {
						mode = Paste;
						break;
					}
					if (key(Enter)) {
						runSimulation();
						mode = View;
						tick = 0;
						playing = true;
						frameCount = 10; // hack
						break;
					}
				} while (false);
			case Write(cell):
				editText();
				cell.text = textInput == "" ? " " : textInput;
				if (key(Escape) || mouse.dright == 1) {
					cell.text = "";
					mode = Edit;
				} else if (key("e") || mouse.dleft == 1) {
					cell.text = "";
					cell.data = Cell.parseData(textInput);
					mode = Edit;
				}
			case Paste:
				if (mouse.dright == 1 || key(Escape))
					mode = Edit;
				final cell = cellAt(f, mouse.pos);
				if (cell != null && mouse.dleft == 1) {
					cursorI = cell.i;
					cursorJ = cell.j;
					pasteAt(f, cell.i, cell.j);
					mode = Edit;
				}
			case View:
				if (key(Space))
					playing = !playing;
				final multiplier = kb.isControlDown() ? 100 : kb.isShiftDown() ? 10 : 1;
				if (key("a"))
					tick = max(0, tick - multiplier);
				if (key("d") || playing && (multiplier > 1 || frameCount % 10 == 0))
					tick = min(w.ticks.length - 1, tick + Math.ceil(multiplier / (playing ? 10 : 1)));
				if (playing && tick == w.ticks.length - 1)
					playing = false;
				if (key("q"))
					tick = 0;
				if (key("e"))
					tick = w.ticks.length - 1;
				if (key(Escape) || key(Enter))
					mode = Edit;
			case Input(index):
				editText();
				if (key("e")) {
					try {
						inputs[index] = textInput;
					} catch (e) {
					}
					mode = Edit;
				}
				if (key(Escape))
					mode = Edit;
		}
	}

	override function draw():Void {
		g.clear(1);
		drawWorld();
	}

	function drawWorld():Void {
		if (mode == View)
			drawField(w.ticks[tick]);
		else
			drawField(w.input);
		drawInfo();
	}

	function drawInfo():Void {
		g.fillColor(1, 0.9);
		g.fillRect(0, 0, 256, 400);
		g.font("Arial", 16, Bold, SansSerif);
		g.fillColor(0);
		g.textBaseline(Top);
		g.textAlign(Left);
		var y = 0.0;
		var lh = 18.0;
		inline function msg(text:String):Void {
			g.fillText(text, 0, y);
			y += lh;
		}
		msg("Mode: " + switch mode {
			case Edit:
				"Edit";
			case Write(cell):
				'Editing (${cell.i}, ${cell.j})';
			case Paste:
				'Paste';
			case View:
				'Viewing Result';
			case Input(index):
				'Editing Input ${"AB".charAt(index)}';
		});
		y += lh * 0.5;
		switch mode {
			case Edit:
				msg("RMB & Drag: Pan");
				msg("Wheel: Zoom");
				y += lh * 0.5;
				msg("LMB & Drag: (De)Select");
				msg("- Hold Shift: Keep Selection");
				msg("ESC: Deselect All");
				y += lh * 0.5;
				msg("WASD, HJKL, Arrows: Move");
				y += lh * 0.5;
				msg("E: Edit Cell");
				y += lh * 0.5;
				msg("DEL: Erase Cells");
				msg("X: Cut Cells");
				msg("C: Copy Cells");
				msg("V: Paste Cells");
				y += lh * 0.5;
				msg("1: Edit Input A (= " + inputs[0].toString() + ")");
				msg("2: Edit Input B (= " + inputs[1].toString() + ")");
				y += lh * 0.5;
				msg("Enter: Run Simulation");
			case Write(_):
				msg("A, B, S: I/O");
				y += lh * 0.5;
				msg("+, -, *, /, %, @: Ops");
				msg("=: Equals");
				msg("!, #: Not Equals");
				y += lh * 0.5;
				msg("HJKL, Arrows: Arrow");
				y += lh * 0.5;
				msg("0-9: Integer");
				y += lh * 0.5;
				msg("Backspace: Erace Last");
				y += lh * 0.5;
				msg("E, LMB: Confirm");
				msg("ESC, RMB: Cancel");
			case Paste:
				msg("LMB: Confirm");
				msg("RMB, ESC: Cancel");
			case View:
				if (w.failReason != "") {
					g.fillColor(1, 0, 0);
					for (err in w.failReason.trim().split("\n")) {
						msg(err);
					}
					y += lh * 0.5;
					g.fillColor(0);
				}
				msg("Showing Tick " + (tick + 1) + "/" + w.ticks.length);
				msg("time = " + (w.ticks[tick].t + 1));
				y += lh * 0.5;
				final kb = input.keyboard;
				final postfix = kb.isControlDown() ? " x100" : kb.isShiftDown() ? " x10" : "";
				msg("A: Prev Tick " + postfix);
				msg("D: Next Tick " + postfix);
				msg("Q: First Tick");
				msg("E: Last Tick");
				msg("Space: Play/Pause");
				y += lh * 0.5;
				msg("Hold Shift: x10 faster");
				msg("Hold Ctrl: x100 faster");
				y += lh * 0.5;
				msg("Enter: Back to Edit");
				msg("ESC: Back to Edit");
			case Input(index):
				msg("AB".charAt(index) + " = " + textInput);
				y += lh * 0.5;
				msg("E: Confirm");
				msg("ESC: Cancel");
		}
	}

	function drawField(f:Field):Void {
		g.strokeColor(0);
		g.textAlign(Center);
		g.textBaseline(Middle);
		g.font("Courier New", 0.8, Bold, Monospace);
		g.save();
		g.translate(pot.width * 0.5, pot.height * 0.5);
		g.scale(scale, scale);
		g.translate(-center.x, -center.y);
		g.strokeWidth(0.02);
		for (i in 0...SIZE) {
			for (j in 0...SIZE) {
				g.save();
				g.translate(j, i);
				drawCell(f.cellAt(i, j));
				g.restore();
			}
		}

		if (mode == Paste) {
			final wpos = toWorld(input.mouse.pos);
			for (c in copiedCells) {
				g.save();
				final j = Math.floor(c.j + wpos.x);
				final i = Math.floor(c.i + wpos.y);
				g.translate(j, i);
				drawCell(c, 0.5);
				g.restore();
			}
		}

		if (mode != View) {
			g.strokeWidth(0.1);
			g.strokeColor(0);
			g.strokeRect(cursorJ, cursorI, 1, 1);
		}

		g.restore();
	}

	extern inline function drawCell(cell:Cell, alpha:Float = 1):Void {
		if (cell.selected) {
			g.fillColor(0.75, 0.75, 1, alpha);
		} else {
			g.fillColor(1, alpha);
		}
		g.fillRect(0, 0, 1, 1);
		switch mode {
			case Write(c):
				if (c == cell) {
					g.fillColor(1, 0.5, 0.5, alpha);
					g.fillRect(0, 0, 1, 1);
				}
			case _:
		}
		g.strokeColor(0, alpha);
		g.strokeRect(0, 0, 1, 1);
		final text = cell.text != "" ? cell.text : cell.data.toString();
		if (text != "" && text != " ") {
			g.fillColor(0, alpha);
			g.fillText(text, 0.5, 0.5, 1);
		}
	}

	static function main():Void {
		new Main(cast Browser.document.getElementById("canvas"), true, true);
	}
}
