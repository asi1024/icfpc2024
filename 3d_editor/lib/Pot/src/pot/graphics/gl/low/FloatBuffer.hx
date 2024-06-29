package pot.graphics.gl.low;

import js.html.webgl.Buffer;
import js.html.webgl.GL2;
import js.lib.Float32Array;

class FloatBuffer extends GLObject {
	static inline final USE_SUB_DATA:Bool = false;

	final kind:BufferKind;
	final buffer:Buffer;

	public var length(default, null):Int = 0;
	public var capacity(default, null):Int = 0;

	public function new(gl:GL2, kind:BufferKind) {
		super(gl);
		this.kind = kind;
		buffer = gl.createBuffer();
	}

	public function getRawBuffer():Buffer {
		return buffer;
	}

	public function upload(data:Float32Array, usage:BufferUsage):Void {
		length = data.length;
		if (!USE_SUB_DATA || length > capacity) {
			gl.bindBuffer(kind, buffer);
			gl.bufferData(kind, data, usage);
			gl.bindBuffer(kind, null);
			capacity = length;
		} else {
			gl.bindBuffer(kind, buffer);
			gl.bufferSubData(kind, 0, data, 0);
			gl.bindBuffer(kind, null);
		}
	}

	public function subUpload(offset:Int, data:Float32Array):Void {
		assert(offset + data.length <= capacity);
		gl.bindBuffer(kind, buffer);
		gl.bufferSubData(kind, offset * Float32Array.BYTES_PER_ELEMENT, data, 0);
		gl.bindBuffer(kind, null);
		length = max(length, offset + data.length);
	}

	public function download(offset:Int, dst:Float32Array):Void {
		gl.bindBuffer(kind, buffer);
		gl.getBufferSubData(kind, offset * Float32Array.BYTES_PER_ELEMENT, dst);
		gl.bindBuffer(kind, null);
	}

	public function sync():Void {
		gl.bindBuffer(kind, buffer);
		gl.getBufferSubData(kind, 0, new Float32Array(1));
		gl.bindBuffer(kind, null);
	}

	public function vertexAttribPointer(location:Int, size:Int, stride:Int, offset:Int):Void {
		assert(kind == ArrayBuffer);
		gl.bindBuffer(kind, buffer);
		gl.vertexAttribPointer(location, size, GL2.FLOAT, false, stride, offset);
		gl.bindBuffer(kind, null);
	}

	function disposeImpl():Void {
		gl.deleteBuffer(buffer);
	}
}
