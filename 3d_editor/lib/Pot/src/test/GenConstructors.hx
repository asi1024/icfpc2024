package test;

using Lambda;
using StringTools;

private class Type {
	public final name:String;
	public final name2:String;
	public final size:Int;

	public function new(name:String, size:Int, name2:String = "") {
		this.name = name;
		this.name2 = name2;
		this.size = size;
	}
}

class GenConstructors {
	public static function gen():Void {
		final types = [
			new Type("Comp1", 1),
			new Type("Comp2", 2),
			new Type("Comp3", 3),
			new Type("Comp4", 4),

			// new Type("Float", 1),  new Type("Vec2", 2),  new Type("Vec3", 3),  new Type("Vec4", 4),
			//   new Type("Int", 1), new Type("IVec2", 2), new Type("IVec3", 3), new Type("IVec4", 4),
			//  new Type("UInt", 1), new Type("UVec2", 2), new Type("UVec3", 3), new Type("UVec4", 4),
			//  new Type("Bool", 1), new Type("BVec2", 2), new Type("BVec3", 3), new Type("BVec4", 4),
		];
		final cs = [
			// new Type("float", 1, "Float"),
			// new Type("vec2", 2, "Vec2"),
			// new Type("vec3", 3, "Vec3"),
			// new Type("vec4", 4, "Vec4"),
			// new Type("int", 1, "Int"),
			// new Type("ivec2", 2, "IVec2"),
			// new Type("ivec3", 3, "IVec3"),
			// new Type("ivec4", 4, "IVec4"),
			// new Type("uint", 1, "UInt"),
			// new Type("uvec2", 2, "UVec2"),
			// new Type("uvec3", 3, "UVec3"),
			// new Type("uvec4", 4, "UVec4"),
			// new Type("bool", 1, "Bool"),
			// new Type("bvec2", 2, "BVec2"),
			// new Type("bvec3", 3, "BVec3"),
			// new Type("bvec4", 4, "BVec4"),
			new Type("mat2", 4, "Mat2"),
			new Type("mat3", 9, "Mat3"),
			new Type("mat4", 16, "Mat4"),
			new Type("mat2x3", 6, "Mat2x3"),
			new Type("mat3x2", 6, "Mat3x2"),
			new Type("mat2x4", 8, "Mat2x4"),
			new Type("mat4x2", 8, "Mat4x2"),
			new Type("mat3x4", 12, "Mat3x4"),
			new Type("mat4x3", 12, "Mat4x3"),
		];
		final funcs = [];
		for (c in cs) {
			final result = [];
			dfs(c.size, [], types, result);
			for (path in result) {
				final args = path.mapi((i, arg) -> String.fromCharCode("a".charCodeAt(0) + i) + ":" + arg).join(", ");
				funcs.push('overload static function ${c.name}($args):${c.name2};');
			}
		}
		trace(funcs.join("\n"));
	}

	static function dfs(left:Int, path:Array<String>, types:Array<Type>, result:Array<Array<String>>):Void {
		if (left == 0) {
			result.push(path.copy());
			return;
		}
		for (type in types) {
			if (left - type.size >= 0) {
				left -= type.size;
				path.push(type.name);
				dfs(left, path, types, result);
				path.pop();
				left += type.size;
			}
		}
	}
}
