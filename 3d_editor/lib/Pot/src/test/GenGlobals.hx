package test;

import js.lib.Set;

using StringTools;
using Lambda;

class GenGlobals {
	public static function gen():Void {
		// https://www.khronos.org/files/webgl20-reference-guide.pdf
		// T: float, vec2, vec3, vec4
		// TI: int, ivec2, ivec3, ivec4
		// TU: uint, uvec2, uvec3, uvec4
		// TB: bool, bvec2, bvec3, bvec4
		// V: vec2, vec3, vec4
		// VI: ivec2, ivec3, ivec4
		// VU: uvec2, uvec3, uvec4
		// VB: bvec2, bvec3, bvec4
		// M: mat2, mat3, mat4, mat2x3, mat3x2, mat2x4, mat4x2, mat3x4, mat4x3
		// g: "", "i", "u"
		final funcs = [
			"radians:T degrees:T",
			"degrees:T radians:T",
			"sin:T angle:T",
			"cos:T angle:T",
			"tan:T angle:T",
			"asin:T x:T",
			"acos:T x:T",
			"atan:T y:T x:T",
			"atan:T yOverX:T",
			"sinh:T x:T",
			"cosh:T x:T",
			"tanh:T x:T",
			"asinh:T x:T",
			"acosh:T x:T",
			"atanh:T x:T",

			"pow:T x:T y:T",
			"exp:T x:T",
			"log:T x:T",
			"exp2:T x:T",
			"log2:T x:T",
			"sqrt:T x:T",
			"inversesqrt:T x:T",

			"abs:T x:T",
			"abs:TI x:TI",
			"sign:T x:T",
			"sign:TI x:TI",
			"floor:T x:T",
			"trunc:T x:T",
			"round:T x:T",
			"roundEven:T x:T",
			"ceil:T x:T",
			"fract:T x:T",
			"mod:T x:T y:T",
			"mod:T x:T y:float",
			// "modf:T x:T out i:T", // just ignore this one
			"min:T x:T y:T",
			"min:TI x:TI y:TI",
			"min:TU x:TU y:TU",
			"min:T x:T y:float",
			"min:TI x:TI y:int",
			"min:TU x:TU y:uint",
			"max:T x:T y:T",
			"max:TI x:TI y:TI",
			"max:TU x:TU y:TU",
			"max:T x:T y:float",
			"max:TI x:TI y:int",
			"max:TU x:TU y:uint",
			"clamp:T x:T minVal:T maxVal:T",
			"clamp:TI x:TI minVal:TI maxVal:TI",
			"clamp:TU x:TU minVal:TU maxVal:TU",
			"clamp:T x:T minVal:float maxVal:float",
			"clamp:TI x:TI minVal:int maxVal:int",
			"clamp:TU x:TU minVal:uint maxVal:uint",
			"mix:T x:T y:T a:T",
			"mix:T x:T y:T a:float",
			"mix:T x:T y:T a:TB",
			"step:T edge:T x:T",
			"step:T edge:float x:T",

			"smoothstep:T edge0:T edge1:T x:T",
			"smoothstep:T edge0:float edge1:float x:T",
			"isnan:TB x:T",
			"isinf:TB x:T",
			"floatBitsToInt:TI value:T",
			"floatBitsToUint:TU value:T",
			"intBitsToFloat:T value:TI",
			"uintBitsToFloat:T value:TU",

			"packSnorm2x16:uint v:vec2",
			"packUnorm2x16:uint v:vec2",
			"unpackSnorm2x16:vec2 p:uint",
			"unpackUnorm2x16:vec2 p:uint",
			"packHalf2x16:uint v:vec2",
			"unpackHalf2x16:vec2 p:uint",

			"length:float x:T",
			"distance:float p0:T p1:T",
			"dot:float x:T y:T",
			"cross:vec3 x:vec3 y:vec3",
			"normalize:T x:T",
			"faceforward:T n:T i:T nRef:T",
			"reflect:T i:T n:T",
			"refract:T i:T n:T eta:float",

			"matrixCompMult:M x:M y:M",
			"outerProduct:mat2 c:vec2 r:vec2",
			"outerProduct:mat3 c:vec3 r:vec3",
			"outerProduct:mat4 c:vec4 r:vec4",
			"outerProduct:mat2x3 c:vec3 r:vec2",
			"outerProduct:mat3x2 c:vec2 r:vec3",
			"outerProduct:mat2x4 c:vec4 r:vec2",
			"outerProduct:mat4x2 c:vec2 r:vec4",
			"outerProduct:mat3x4 c:vec4 r:vec3",
			"outerProduct:mat4x3 c:vec3 r:vec4",
			"transpose:mat2 m:mat2",
			"transpose:mat3 m:mat3",
			"transpose:mat4 m:mat4",
			"transpose:mat2x3 m:mat3x2",
			"transpose:mat3x2 m:mat2x3",
			"transpose:mat2x4 m:mat4x2",
			"transpose:mat4x2 m:mat2x4",
			"transpose:mat3x4 m:mat4x3",
			"transpose:mat4x3 m:mat3x4",
			"determinant:float m:mat2",
			"determinant:float m:mat3",
			"determinant:float m:mat4",
			"inverse:mat2 m:mat2",
			"inverse:mat3 m:mat3",
			"inverse:mat4 m:mat4",

			"lessThan:VB x:V y:V",
			"lessThan:VB x:VI y:VI",
			"lessThan:VB x:VU y:VU",
			"lessThanEqual:VB x:V y:V",
			"lessThanEqual:VB x:VI y:VI",
			"lessThanEqual:VB x:VU y:VU",
			"greaterThan:VB x:V y:V",
			"greaterThan:VB x:VI y:VI",
			"greaterThan:VB x:VU y:VU",
			"greaterThanEqual:VB x:V y:V",
			"greaterThanEqual:VB x:VI y:VI",
			"greaterThanEqual:VB x:VU y:VU",
			"equal:VB x:V y:V",
			"equal:VB x:VI y:VI",
			"equal:VB x:VU y:VU",
			"equal:VB x:VB y:VB",
			"notEqual:VB x:V y:V",
			"notEqual:VB x:VI y:VI",
			"notEqual:VB x:VU y:VU",
			"notEqual:VB x:VB y:VB",
			"any:bool x:VB",
			"all:bool x:VB",
			"not:VB x:VB",

			"textureSize:ivec2 sampler:gsampler2D lod:int",
			"textureSize:ivec3 sampler:gsampler3D lod:int",
			"textureSize:ivec2 sampler:gsamplerCube lod:int",
			"textureSize:ivec2 sampler:sampler2DShadow lod:int",
			"textureSize:ivec2 sampler:samplerCubeShadow lod:int",
			"textureSize:ivec3 sampler:gsampler2DArray lod:int",
			"textureSize:ivec3 sampler:sampler2DArrayShadow lod:int",

			"texture:gvec4 sampler:gsampler2D P:vec2 ?bias:float",
			"texture:gvec4 sampler:gsampler3D P:vec3 ?bias:float",
			"texture:gvec4 sampler:gsamplerCube P:vec3 ?bias:float",
			"texture:float sampler:sampler2DShadow P:vec3 ?bias:float",
			"texture:float sampler:samplerCubeShadow P:vec4 ?bias:float",
			"texture:gvec4 sampler:gsampler2DArray P:vec3 ?bias:float",
			"texture:float sampler:sampler2DArrayShadow P:vec4",

			"textureProj:gvec4 sampler:gsampler2D P:vec3 ?bias:float",
			"textureProj:gvec4 sampler:gsampler2D P:vec4 ?bias:float",
			"textureProj:gvec4 sampler:gsampler3D P:vec4 ?bias:float",
			"textureProj:float sampler:sampler2DShadow P:vec4 ?bias:float",

			"textureLod:gvec4 sampler:gsampler2D P:vec2 lod:float",
			"textureLod:gvec4 sampler:gsampler3D P:vec3 lod:float",
			"textureLod:gvec4 sampler:gsamplerCube P:vec3 lod:float",
			"textureLod:float sampler:sampler2DShadow P:vec3 lod:float",
			"textureLod:gvec4 sampler:gsampler2DArray P:vec3 lod:float",

			"textureOffset:gvec4 sampler:gsampler2D P:vec2 offset:ivec2 ?bias:float",
			"textureOffset:gvec4 sampler:gsampler3D P:vec3 offset:ivec3 ?bias:float",
			"textureOffset:float sampler:sampler2DShadow P:vec3 offset:ivec2 ?bias:float",
			"textureOffset:gvec4 sampler:gsampler2DArray P:vec3 offset:ivec2 ?bias:float",

			"texelFetch:gvec4 sampler:gsampler2D P:ivec2 lod:int",
			"texelFetch:gvec4 sampler:gsampler3D P:ivec3 lod:int",
			"texelFetch:gvec4 sampler:gsampler2DArray P:ivec3 lod:int",

			"texelFetchOffset:gvec4 sampler:gsampler2D P:ivec2 lod:int offset:ivec2",
			"texelFetchOffset:gvec4 sampler:gsampler3D P:ivec3 lod:int offset:ivec3",
			"texelFetchOffset:gvec4 sampler:gsampler2DArray P:ivec3 lod:int offset:ivec2",

			"textureProjOffset:gvec4 sampler:gsampler2D P:vec3 offset:ivec2 ?bias:float",
			"textureProjOffset:gvec4 sampler:gsampler2D P:vec4 offset:ivec2 ?bias:float",
			"textureProjOffset:gvec4 sampler:gsampler3D P:vec4 offset:ivec3 ?bias:float",
			"textureProjOffset:float sampler:sampler2DShadow P:vec4 offset:ivec2 ?bias:float",

			"textureLodOffset:gvec4 sampler:gsampler2D P:vec2 lod:float offset:ivec2",
			"textureLodOffset:gvec4 sampler:gsampler3D P:vec3 lod:float offset:ivec3",
			"textureLodOffset:float sampler:sampler2DShadow P:vec3 lod:float offset:ivec2",
			"textureLodOffset:gvec4 sampler:gsampler2DArray P:vec3 lod:float offset:ivec2",

			"textureProjLod:gvec4 sampler:gsampler2D P:vec3 lod:float",
			"textureProjLod:gvec4 sampler:gsampler2D P:vec4 lod:float",
			"textureProjLod:gvec4 sampler:gsampler3D P:vec4 lod:float",
			"textureProjLod:float sampler:sampler2DShadow P:vec4 lod:float",

			"textureProjLodOffset:gvec4 sampler:gsampler2D P:vec3 lod:float offset:ivec2",
			"textureProjLodOffset:gvec4 sampler:gsampler2D P:vec4 lod:float offset:ivec2",
			"textureProjLodOffset:gvec4 sampler:gsampler3D P:vec4 lod:float offset:ivec3",
			"textureProjLodOffset:float sampler:sampler2DShadow P:vec4 lod:float offset:ivec2",

			"textureGrad:gvec4 sampler:gsampler2D P:vec2 dPdx:vec2 dPdy:vec2",
			"textureGrad:gvec4 sampler:gsampler3D P:vec3 dPdx:vec3 dPdy:vec3",
			"textureGrad:gvec4 sampler:gsamplerCube P:vec3 dPdx:vec3 dPdy:vec3",
			"textureGrad:float sampler:sampler2DShadow P:vec3 dPdx:vec2 dPdy:vec2",
			"textureGrad:float sampler:samplerCubeShadow P:vec4 dPdx:vec3 dPdy:vec3",
			"textureGrad:gvec4 sampler:gsampler2DArray P:vec3 dPdx:vec2 dPdy:vec2",
			"textureGrad:float sampler:sampler2DArrayShadow P:vec4 dPdx:vec2 dPdy:vec2",

			"textureGradOffset:gvec4 sampler:gsampler2D P:vec2 dPdx:vec2 dPdy:vec2 offset:ivec2",
			"textureGradOffset:gvec4 sampler:gsampler3D P:vec3 dPdx:vec3 dPdy:vec3 offset:ivec3",
			"textureGradOffset:float sampler:sampler2DShadow P:vec3 dPdx:vec2 dPdy:vec2 offset:ivec2",
			"textureGradOffset:gvec4 sampler:gsampler2DArray P:vec3 dPdx:vec2 dPdy:vec2 offset:ivec2",
			"textureGradOffset:float sampler:sampler2DArrayShadow P:vec4 dPdx:vec2 dPdy:vec2 offset:ivec2",

			"textureProj:gvec4 sampler:gsampler2D P:vec3 dPdx:vec2 dPdy:vec2",
			"textureProj:gvec4 sampler:gsampler2D P:vec4 dPdx:vec2 dPdy:vec2",
			"textureProj:gvec4 sampler:gsampler3D P:vec4 dPdx:vec3 dPdy:vec3",
			"textureProj:float sampler:sampler2DShadow P:vec4 dPdx:vec2 dPdy:vec2",

			"textureProjOffset:gvec4 sampler:gsampler2D P:vec3 dPdx:vec2 dPdy:vec2 offset:ivec2",
			"textureProjOffset:gvec4 sampler:gsampler2D P:vec4 dPdx:vec2 dPdy:vec2 offset:ivec2",
			"textureProjOffset:gvec4 sampler:gsampler3D P:vec4 dPdx:vec3 dPdy:vec3 offset:ivec3",
			"textureProjOffset:float sampler:sampler2DShadow P:vec4 dPdx:vec2 dPdy:vec2 offset:ivec2",

			"dFdx:T p:T",
			"dFdy:T p:T",
			"fwidth:T p:T",
		];

		final funcsAll = [for (func in new Set<String>(funcs.flatMap(line -> {
			var all = [line];
			all = all.flatMap(f -> {
				if (f.contains(":T")) {
					[
						f.replace(":TI", ":int").replace(":TU", ":uint").replace(":TB", ":bool").replace(":T", ":float"),
						f.replace(":TI", ":ivec2").replace(":TU", ":uvec2").replace(":TB", ":bvec2").replace(":T", ":vec2"),
						f.replace(":TI", ":ivec3").replace(":TU", ":uvec3").replace(":TB", ":bvec3").replace(":T", ":vec3"),
						f.replace(":TI", ":ivec4").replace(":TU", ":uvec4").replace(":TB", ":bvec4").replace(":T", ":vec4")
					];
				} else {
					[f];
				}
			});
			all = all.flatMap(f -> {
				if (f.contains(":V")) {
					[
						f.replace(":VI", ":ivec2").replace(":VU", ":uvec2").replace(":VB", ":bvec2").replace(":V", ":vec2"),
						f.replace(":VI", ":ivec3").replace(":VU", ":uvec3").replace(":VB", ":bvec3").replace(":V", ":vec3"),
						f.replace(":VI", ":ivec4").replace(":VU", ":uvec4").replace(":VB", ":bvec4").replace(":V", ":vec4")
					];
				} else {
					[f];
				}
			});
			all = all.flatMap(f -> {
				if (f.contains(":M")) {
					[
						f.replace(":M", ":mat2"),
						f.replace(":M", ":mat3"),
						f.replace(":M", ":mat4"),
						f.replace(":M", ":mat2x3"),
						f.replace(":M", ":mat3x2"),
						f.replace(":M", ":mat2x4"),
						f.replace(":M", ":mat4x2"),
						f.replace(":M", ":mat3x4"),
						f.replace(":M", ":mat4x3"),
					];
				} else {
					[f];
				}
			});
			all = all.flatMap(f -> {
				if (f.contains(":g")) {
					[f.replace(":g", ":"), f.replace(":g", ":i"), f.replace(":g", ":u")];
				} else {
					[f];
				}
			});
			all;
		}))) func];

		final defs = funcsAll.map(func -> {
			final names = func.replace(":", ":G").split(" ").map(s -> s.split(":"));
			final name = names[0][0];
			final type = names[0][1];
			final args = names.slice(1).map(s -> s.join(":")).join(", ");
			'overload static function $name($args):$type;';
		}).join("\n");

		trace(defs);
	}
}
