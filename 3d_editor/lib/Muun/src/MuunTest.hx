import muun.la.ImVec2;
import muun.la.Vec4;
import muun.la.Mat2;
import muun.la.Vec2;
import muun.la.Quat;
import muun.la.Mat3;
import muun.la.Vec3;

class MuunTest {
	public function new() {
		testVec2();
		testVec3();
		testVec4();
		testMat2();
		testMat3();
		testQuat();
	}

	function testVec2():Void {
		var a = Vec2.of(1, 2);
		var b = Vec2.of(3, 4);
		var c = (5 / (2 * a + 3 * b) * b.normalized) * 114 / a.dot(b) * a + 514 * b;
		var d = a.cross(b);
		var ima:ImVec2 = a;
		var imb = ima + ima;
		trace(c.x + " " + a.length + " " + d);
	}

	function testVec3():Void {
		var a = Vec3.of(1, 2, 3);
		var b = Vec3.of(4, 5, 6);
		var c = (5 / (2 * a + 3 * b) * b.normalized) * 114 / a.dot(b) * a + 514 * b;
		var d = a.cross(b).cross(c).cross(a);
		a <<= d.copy();
		trace(c.x + " " + a.length);
	}

	function testVec4():Void {
		var a = Vec4.of(1, 2, 3, 4);
		var b = Vec4.of(4, 5, 6, 7);
		var c = (5 / (2 * a + 3 * b) * b.normalized) * 114 / a.dot(b) * a + 514 * b;
		a <<= c.yzxw;
		a <<= a.yzxw;
		trace(c.x + " " + a.length);
	}

	function testMat2():Void {
		var a = Mat2.id;
		var b = a * 3;
		var c = (5 / (2 * a + 3 * b + Mat2.one) * b.t) * 114 / (a * b + Mat2.one) * a + 514 * b;
		c <<= c.t;
		c <<= c.t;
		c <<= c.t;
		c <<= c.t;
		c <<= c.t;
		var d = Mat2.rot(3.34) * c;
		a <<= d.copy();
		trace(c.e00 + " " + (c * c.t).tr);
	}

	function testMat3():Void {
		var a = Mat3.id;
		var b = Vec3.of(3, 3, 4).diag;
		var c = (5 / (2 * a + 3 * b + Mat3.one) * b.t) * 114 / (a * b + Mat3.one) * a + 514 * b;
		c = c.t;
		c = c.t;
		c = c.t;
		c = c.t;
		c = c.t;
		var d = Mat3.rot(3.34, Vec3.ex) * c;
		a <<= d.copy();
		trace(c.e00 + " " + (c * c.t).tr);
	}

	function testQuat():Void {
		trace(Quat.arc(Vec3.ez, Vec3.ex).length);
	}

	static function main() {
		new MuunTest();
	}
}
