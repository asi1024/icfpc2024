import js.Syntax;

@:native("BigIntWrapper")
abstract BigInt(BigIntData) {
	@:from
	public static function fromInt(val:Int):BigInt {
		return cast Syntax.code("BigInt({0})", val);
	}

	@:from
	public static function fromString(val:String):BigInt {
		return cast Syntax.code("BigInt({0})", val);
	}

	public function toString():String {
		return cast Syntax.code("{0}.toString()", this);
	}

	@:op(A + B)
	static function add(a:BigInt, b:BigInt):BigInt {
		return Syntax.code("{0} + {1}", a, b);
	}

	@:op(A - B)
	static function sub(a:BigInt, b:BigInt):BigInt {
		return Syntax.code("{0} - {1}", a, b);
	}

	@:op(A * B)
	static function mul(a:BigInt, b:BigInt):BigInt {
		return Syntax.code("{0} * {1}", a, b);
	}

	@:op(A / B)
	static function div(a:BigInt, b:BigInt):BigInt {
		return Syntax.code("{0} / {1}", a, b);
	}

	@:op(A % B)
	static function mod(a:BigInt, b:BigInt):BigInt {
		return Syntax.code("{0} % {1}", a, b);
	}

	@:op(-A)
	static function minus(a:BigInt):BigInt {
		return Syntax.code("-{0}", a);
	}

	@:op(A == B)
	static function eq(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} == {1}", a, b);
	}

	@:op(A != B)
	static function neq(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} != {1}", a, b);
	}

	@:op(A < B)
	static function lt(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} < {1}", a, b);
	}

	@:op(A > B)
	static function gt(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} > {1}", a, b);
	}

	@:op(A <= B)
	static function leq(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} <= {1}", a, b);
	}

	@:op(A >= B)
	static function geq(a:BigInt, b:BigInt):Bool {
		return Syntax.code("{0} >= {1}", a, b);
	}

	public function toInt():Int {
		return Syntax.code("Number({0})", this);
	}
}

@:native("BigInt")
private extern class BigIntData {
}
