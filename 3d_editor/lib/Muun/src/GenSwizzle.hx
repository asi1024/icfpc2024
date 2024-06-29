class GenSwizzle {
	public static function gen(comps:Array<String>):String {
		var res1 = "";
		var res2 = "";
		for (a in comps) {
			for (b in comps) {
				res1 += 'public var $a$b(get, never):Vec2;\n';
				res2 += 'extern inline function get_$a$b():Vec2
	return Vec2.of(this.$a, this.$b);\n';
			}
		}
		for (a in comps) {
			for (b in comps) {
				for (c in comps) {
					res1 += 'public var $a$b$c(get, never):Vec3;\n';
					res2 += 'extern inline function get_$a$b$c():Vec3
	return Vec3.of(this.$a, this.$b, this.$c);\n';
				}
			}
		}
		for (a in comps) {
			for (b in comps) {
				for (c in comps) {
					for (d in comps) {
						res1 += 'public var $a$b$c$d(get, never):Vec4;\n';
						res2 += 'extern inline function get_$a$b$c$d():Vec4
	return Vec4.of(this.$a, this.$b, this.$c, this.$d);\n';
					}
				}
			}
		}
		return res1 + res2;
	}
}
