class Body {
	construct new() { }

	x=(value) {_x=value}
	x {_x}

	y=(value) {_y=value}
	y {_y}

	z=(value) {_z=value}
	z {_z}

	vx=(value) {_vx=value}
	vx {_vx}

	vy=(value) {_vy=value}
	vy {_vy}

	vz=(value) {_vz=value}
	vz {_vz}

	mass=(value) {_mass=value}
	mass {_mass}

}

var PI = 3.141592653589793
var SOLAR_MASS = 4 * PI * PI
var DAYS_PER_YEAR = 365.24

class Nope {
	construct new() {
		var sun = Body.new()
		sun.x = 0.0
		sun.y = 0.0
		sun.z = 0.0
		sun.vx = 0.0
		sun.vy = 0.0
		sun.vz = 0.0
		sun.mass = SOLAR_MASS

		var jupiter = Body.new()
		jupiter.x = 4.84143144246472090e00
		jupiter.y = -1.16032004402742839e00
		jupiter.z = -1.03622044471123109e-01
		jupiter.vx = 1.66007664274403694e-03 * DAYS_PER_YEAR
		jupiter.vy = 7.69901118419740425e-03 * DAYS_PER_YEAR
		jupiter.vz = -6.90460016972063023e-05 * DAYS_PER_YEAR
		jupiter.mass = 9.54791938424326609e-04 * SOLAR_MASS

		var saturn = Body.new()
		saturn.x = 8.34336671824457987e00
		saturn.y = 4.12479856412430479e00
		saturn.z = -4.03523417114321381e-01
		saturn.vx = -2.76742510726862411e-03 * DAYS_PER_YEAR
		saturn.vy = 4.99852801234917238e-03 * DAYS_PER_YEAR
		saturn.vz = 2.30417297573763929e-05 * DAYS_PER_YEAR
		saturn.mass = 2.85885980666130812e-04 * SOLAR_MASS

		var uranus = Body.new()
		uranus.x = 1.28943695621391310e01
		uranus.y = -1.51111514016986312e01
		uranus.z = -2.23307578892655734e-01
		uranus.vx = 2.96460137564761618e-03 * DAYS_PER_YEAR
		uranus.vy = 2.37847173959480950e-03 * DAYS_PER_YEAR
		uranus.vz = -2.96589568540237556e-05 * DAYS_PER_YEAR
		uranus.mass = 4.36624404335156298e-05 * SOLAR_MASS

		var neptune = Body.new()
		neptune.x = 1.53796971148509165e01
		neptune.y = -2.59193146099879641e01
		neptune.z = 1.79258772950371181e-01
		neptune.vx = 2.68067772490389322e-03 * DAYS_PER_YEAR
		neptune.vy = 1.62824170038242295e-03 * DAYS_PER_YEAR
		neptune.vz = -9.51592254519715870e-05 * DAYS_PER_YEAR
		neptune.mass = 5.15138902046611451e-05 * SOLAR_MASS

		_bodies = [sun, jupiter, saturn, uranus, neptune]
	}
	offsetMomentum() {
		var px = 0
		var py = 0
		var pz = 0
		for (body in _bodies) {
			var bim = body.mass
			px = px + (body.vx * bim)
			py = py + (body.vy * bim)
			pz = pz + (body.vz * bim)
		}
		_bodies[1].vx = -px / SOLAR_MASS
		_bodies[1].vy = -py / SOLAR_MASS
		_bodies[1].vz = -pz / SOLAR_MASS
	}
	energy() {
		var e = 0
		for (i in 1..._bodies.count) {
			var bi = _bodies[i]
			var vx = bi.vx
			var vy = bi.vy
			var vz = bi.vz
			var bim = bi.mass
			e = e + (0.5 * bim * (vx*vx + vy*vy + vz*vz))
			for (j in i+1..._bodies.count) {
				var bj = _bodies[j]
				var dx = bi.x - bj.x
				var dy = bi.y - bj.y
				var dz = bi.z - bj.z
				var distance = (dx*dx + dy*dy + dz*dz).sqrt
				e = e - ((bim * bj.mass) / distance)
			}
		}
		return e
	}
	advance(dt) {
		for (i in 0..._bodies.count) {
			var ibody = _bodies[i]
			for (j in i+1..._bodies.count) {
				var jbody = _bodies[j]
				var dx = ibody.x - jbody.x
				var dy = ibody.y - jbody.y
				var dz = ibody.z - jbody.z
				var dist2 = dx*dx + dy*dy + dz*dz
				var mag = dist2.sqrt
				mag = dt / (mag * dist2)
				var bm = jbody.mass * mag
				ibody.vx = ibody.vx - (dx * bm)
				ibody.vy = ibody.vy - (dy * bm)
				ibody.vz = ibody.vz - (dz * bm)
				bm = ibody.mass*mag
				jbody.vx = jbody.vx + (dx * bm)
				jbody.vy = jbody.vy + (dy * bm)
				jbody.vz = jbody.vz + (dz * bm)
			}
			ibody.x = ibody.x + dt * ibody.vx
			ibody.y = ibody.y + dt * ibody.vy
			ibody.z = ibody.z + dt * ibody.vz
		}
	}
	static main(steps) {
		var system = Nope.new()
		system.offsetMomentum()
		system.energy()
		for (i in 0...steps) {
			system.advance(0.01)
		}
		system.energy()
	}
}
