package test;

import js.lib.Set;

using StringTools;
using Lambda;

class GenSwizzle {
	public static function gen():Void {
		final vals2 = [["x", "y"], ["r", "g"], ["s", "t"]];
		final vals3 = [["x", "y", "z"], ["r", "g", "b"], ["s", "t", "p"]];
		final vals4 = [["x", "y", "z", "w"], ["r", "g", "b", "a"], ["s", "t", "p", "q"]];

		final ftypes = ["Float", "Vec2", "Vec3", "Vec4"];
		final itypes = ["Int", "IVec2", "IVec3", "IVec4"];
		final utypes = ["UInt", "UVec2", "UVec3", "UVec4"];
		final btypes = ["Bool", "BVec2", "BVec3", "BVec4"];
		final etypes = ["T1", "T2", "T3", "T4"];

		for (i in 0...3) {
			final vals = [vals2, vals3, vals4][i];
			final types = etypes;

			final res = [[], [], []];
			for (comps in vals) {
				for (a in comps) {
					param(res, types, a);
					for (b in comps) {
						param(res, types, a + b);
						for (c in comps) {
							param(res, types, a + b + c);
							for (d in comps) {
								param(res, types, a + b + c + d);
							}
						}
					}
				}
			}
			trace(StringTools.rpad("@:forward(" + res[0].join(",") + ")\n" + res[1].join("") + "\n" + res[2].join("") + "\n", " ", 12000));
			trace("above is n=" + (i + 2));
		}
	}

	static function param(res:Array<Array<String>>, types:Array<String>, name:String):Void {
		final n = name.length;
		final comps = name.split("").map(c -> "xyzwrgbastpq".indexOf(c) % 4);
		final lvalue = comps.foreach(c -> comps.count(c2 -> c == c2) == 1);
		final type = types[n - 1];
		// final compsStr = comps.join(",");
		// funcs[0].push('public var $name(get, ${lvalue ? "set" : "never"}):$type;');
		// funcs[1].push('extern inline function get_$name() return gw(this, $compsStr);');
		// if (lvalue)
		// 	funcs[1].push('extern inline function set_$name(v:$type) return sw(this, $compsStr, v);');

		res[0].push('$name');
		if (lvalue)
			res[1].push('var $name:$type;');
		else
			res[2].push('final $name:$type;');
	}
}
