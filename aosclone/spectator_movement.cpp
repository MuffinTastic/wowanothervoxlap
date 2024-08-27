void PlayerType::boxclipmove_spectator()
{
	float f, nx, ny, nz;
	voxlap::point3d op;

	f = fsynctics * 32.0f;

	nx = f*this->velocity.x + this->position.x;
	if (aos_vxl_clipbox_spectator(nx, this->position.y, this->position.z))
		this->velocity.x = 0.0f;

	ny = f*this->velocity.y + this->position.y;
	if (aos_vxl_clipbox_spectator(this->position.x, ny, this->position.z))
		this->velocity.y = 0.0f;

	nz = f*this->velocity.z + this->position.z;
	if (aos_vxl_clipbox_spectator(this->position.x, this->position.y, nz))
		this->velocity.z = 0.0f;

	op = this->position;
	this->position += this->velocity*f;

	if (aos_vxl_clipbox(this->position.x, this->position.y, this->position.z)) {
		this->position = op;
		this->velocity /= 2.0f;
	}

	this->reposition(&this->position);
}

void PlayerType::move_spectator()
{
	float f, f2;

	//move spectator and perform simple physics (momentum, etc)

	f = fsynctics; //player acceleration scalar
	if (this->sprint)
		f *= 2.0f;

	long mov_amount = 0;
	mov_amount += (this->move_for || this->move_back);
	mov_amount += (this->move_left || this->move_right);
	mov_amount += (this->jump || this->crouch);
	switch (mov_amount) {
	case 2:
		f *= SQRT;
		break;
	case 3:
		f *= SQRT3;
		break;
	}

	if (this->move_for) {
		this->velocity.x += this->orientation_for.x*f;
		this->velocity.y += this->orientation_for.y*f;
		this->velocity.z += this->orientation_for.z*f;
	} else if (this->move_back) {
		this->velocity.x -= this->orientation_for.x*f;
		this->velocity.y -= this->orientation_for.y*f;
		this->velocity.z -= this->orientation_for.z*f;
	}
	if (this->move_left) {
		this->velocity.x -= this->orientation_str.x*f;
		this->velocity.y -= this->orientation_str.y*f;
		this->velocity.z -= this->orientation_str.z*f;
	} else if (this->move_right) {
		this->velocity.x += this->orientation_str.x*f;
		this->velocity.y += this->orientation_str.y*f;
		this->velocity.z += this->orientation_str.z*f;
	}
	if (this->jump) {
		this->velocity.x -= this->orientation_hei.x*f;
		this->velocity.y -= this->orientation_hei.y*f;
		this->velocity.z -= this->orientation_hei.z*f;
	} else if (this->crouch) {
		this->velocity.x += this->orientation_hei.x*f;
		this->velocity.y += this->orientation_hei.y*f;
		this->velocity.z += this->orientation_hei.z*f;
	}

	f = fsynctics + 1;
	this->velocity.x /= f;
	this->velocity.y /= f;
	this->velocity.z /= f; //air friction

	this->boxclipmove_spectator();
}


		voxlap::orthorotate(this->orientation_str.z*.1,
			((fmousy_view * PI / 180.) * fsynctics) * config::mouse_sens_y * sens_multiplier,
			((fmousx * PI / 180.) * fsynctics) * config::mouse_sens_x * sens_multiplier,
			&this->orientation_str, &this->orientation_hei, &this->orientation_for);