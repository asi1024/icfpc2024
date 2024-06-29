package test;

import muun.la.Mat3;
import muun.la.Vec3;
import pot.graphics.gl.Texture;
#if 0
import hgsl.ShaderTest;
class PotTest {
	static function main() {
		var hoge:ShaderTest;
	}
}
#elseif 0
import pot.graphics.gl.shader.DefaultShader;
import pot.graphics.gl.shader.DefaultShaderTextured;
class PotTest {
	static function main() {
		trace("no texture version");
		trace("attributes: " + DefaultShader.attributes);
		trace("uniforms: " + DefaultShader.uniforms);
		trace("vertex source: " + DefaultShader.vertexSource);
		trace("fragment source: " + DefaultShader.fragmentSource);
		trace("\n\n\n");
		trace("textured version");
		trace("attributes: " + DefaultShaderTextured.attributes);
		trace("uniforms: " + DefaultShaderTextured.uniforms);
		trace("vertex source: " + DefaultShaderTextured.vertexSource);
		trace("fragment source: " + DefaultShaderTextured.fragmentSource);
	}
}
#else
import hgsl.Global;
import js.Browser;
import pot.core.App;
import pot.graphics.bitmap.Bitmap;
import pot.graphics.gl.Graphics;
import pot.graphics.gl.Shader;
import pot.graphics.gl.shader.DefaultShader;
import pot.util.ImageLoader;

class PotTest extends App {
	var g:Graphics;
	var tex:Texture;
	var x:Float = 0;

	override function setup() {
		pot.frameRate(Fixed(10));

		final b = new Bitmap(400, 300);
		final bg = b.getGraphics();
		bg.clear(0, 0, 1);
		bg.strokeColor(1, 0, 0);
		bg.strokeWidth(10);
		bg.drawLine(10, 10, 200, 100);

		g = new Graphics(canvas);
		g.init3D();
		ImageLoader.loadImages(["tex.png"], a -> {
			tex = g.loadBitmap(a[0]);
			pot.start();
		});
	}

	override function update() {
		x = (x + 4) % pot.width;
	}

	override function draw() {
		final lp1 = Vec3.of(2, 0, 0);
		final lp2 = Vec3.of(2, 0, 0);
		final lp3 = Vec3.of(2, 0, 0);

		lp1 <<= Mat3.rot(frameCount * 0.01, Vec3.ex) * lp1;
		lp1 <<= Mat3.rot(frameCount * 0.015, Vec3.ey) * lp1;
		lp1 <<= Mat3.rot(frameCount * 0.013, Vec3.ez) * lp1;
		lp2 <<= Mat3.rot(frameCount * -0.01, Vec3.ex) * lp2;
		lp2 <<= Mat3.rot(frameCount * -0.008, Vec3.ey) * lp2;
		lp2 <<= Mat3.rot(frameCount * 0.02, Vec3.ez) * lp2;
		lp3 <<= Mat3.rot(frameCount * 0.015, Vec3.ex) * lp3;
		lp3 <<= Mat3.rot(frameCount * -0.01, Vec3.ey) * lp3;
		lp3 <<= Mat3.rot(frameCount * 0.006, Vec3.ez) * lp3;
		lp1.z = lp1.abs().z;
		lp2.z = lp2.abs().z;
		lp3.z = lp3.abs().z;

		g.camera(Vec3.of(0, 0, 5), Vec3.of(0, 0, 0), Vec3.ey);
		g.screen(pot.width, pot.height);
		g.inScene(() -> {
			g.clear(0, 0, 0);

			g.ambientLight(0.2, 0.2, 0.2);
			g.pointLight(1, 0, 0, lp1);
			g.pointLight(0, 1, 0, lp2);
			g.pointLight(0, 0, 1, lp3);
			g.specular(1.0);
			g.texture(tex);
			g.emission(1);

			g.color(1, 0, 0);
			g.pushMatrix();
			g.translate(lp1.x, lp1.y, lp1.z);
			g.sphere(0.05);
			g.popMatrix();

			g.color(0, 1, 0);
			g.pushMatrix();
			g.translate(lp2.x, lp2.y, lp2.z);
			g.sphere(0.05);
			g.popMatrix();

			g.color(0, 0, 1);
			g.pushMatrix();
			g.translate(lp3.x, lp3.y, lp3.z);
			g.sphere(0.05);
			g.popMatrix();

			g.color(1, 1, 1);
			g.emission(0);
			g.rotateX(frameCount * 0.0123);
			g.rotateY(frameCount * 0.01);
			g.box(2, 2, 2);
			g.sphere(1.2);
		});
	}

	static function main() {
		new PotTest(cast Browser.document.getElementById("canvas"));
	}
}
#end
