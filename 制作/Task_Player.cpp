//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XÌú»
	bool  Resource::Initialize()
	{
		this->imgPlayer = DG::Image::Create("./data/image/chara/majo.png");
		this->soundFoot = DM::Sound::CreateSE("./data/audio/playerfoot.wav");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XÌðú
	bool  Resource::Finalize()
	{
		this->imgPlayer.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//uú»v^XN¶¬ÉPñ¾¯s¤
	bool  Object::Initialize()
	{
		//X[p[NXú»
		__super::Initialize(defGroupName, defName, true);	
		//\[XNX¶¬or\[X¤L
		this->res = Resource::Create();

		//f[^ú»
		this->render2D_Priority[1] = 0.6f;
		this->moveSpeed = 2;
		this->stopTime = 10;

		this->seplay = false;
		this->isMove = false;
		this->canTeleport = true;

		this->pos = ML::Vec2(PIXELSIZE / 2, PIXELSIZE / 2);
		this->est = ML::Vec2(0, 0);
		this->hitBase = ML::Box2D(-PIXELSIZE / 2, -PIXELSIZE / 2, PIXELSIZE, PIXELSIZE);
		this->dir = Direction::DOWN;

		//se::LoadFile("playerfoot", "./data/audio/playerfoot.wav");
		//bgm::LoadFile("playerfoot", "./data/audio/playerfoot.mp3");
		//^XNÌ¶¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uI¹v^XNÁÅÉPñ¾¯s¤
	bool  Object::Finalize()
	{
		//f[^^XNðú


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ø«p¬^XNÌ¶¬
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[És¤
	void  Object::UpDate()
	{
		this->Input();
		this->CheckMove();

		if (this->isMove) {
			if (!this->seplay) {
				this->res->soundFoot->Play_Normal(true);
				this->seplay = true;
			}
			this->stopTime = 10;
		}
		else {
			if (this->stopTime < 0) {
				this->seplay = false;
				this->res->soundFoot->Stop();
			}
			this->stopTime--;
		}

		this->AdjustCamera();

		//char c[100];
		//sprintf_s(c, 99, "PlayerPos: (%.1f, %.1f)", this->est.x, this->est.y);
		//ge->qa_Debug->DebugPrint(c);

		this->timeCnt++;
	}
	//-------------------------------------------------------------------
	//uQc`ævPt[És¤
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(-PIXELSIZE / 2, -PIXELSIZE / 2, PIXELSIZE, PIXELSIZE);
		draw.Offset(this->pos);
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		ML::Box2D src(0, 0, PIXELSIZE, PIXELSIZE);
		int animTable[] = { 0, 1, 2, 1 };

		int animFlame;
		if (isMove)
			animFlame = animTable[this->timeCnt / 10 % 4];
		else
			animFlame = animTable[this->timeCnt / 40 % 4];

		switch (this->dir) {
		case Direction::LEFT:
			src = ML::Box2D(animFlame * 32, 32, 32, 32);
			break;
		case Direction::RIGHT:
			src = ML::Box2D(animFlame * 32, 64, 32, 32);
			break;
		case Direction::DOWN:
			src = ML::Box2D(animFlame * 32, 0, 32, 32);
			break;
		case Direction::UP:
			src = ML::Box2D(animFlame * 32, 96, 32, 32);
			break;
		}

		this->res->imgPlayer->Draw(draw, src);
	}


	//-------------------------------------------------------------------
	//J²®
	void Object::AdjustCamera() {		
		if (ge->system.moveCamera) {
			//JÌ[h
			auto inp = ge->in1->GetState();
			if (inp.LStick.BU.on) { ge->camera2D.y -= 5; }
			if (inp.LStick.BD.on) { ge->camera2D.y += 5; }
			if (inp.LStick.BR.on) { ge->camera2D.x += 5; }
			if (inp.LStick.BL.on) { ge->camera2D.x -= 5; }
		}
		else {
			//Êí[h
			int px = ge->camera2D.w / 2;
			int py = ge->camera2D.h / 2;
			int cpx = int(this->pos.x) - px;
			int cpy = int(this->pos.y) - py;
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
		}

		ge->qa_Map->AdjustCameraPos();
	}

	//player input 
	void Object::Input() {
		if (ge->system.gameState != MyPG::MyGameEngine::GAMESTATE::PLAY) return;
		if (ge->system.moveCamera) return;

		if (!isMove) {
			auto inp = ge->in1->GetState();

			//Ú®ÊÝè
			if (inp.LStick.BR.on) { this->est.x = PIXELSIZE; this->dir = Direction::RIGHT; ge->system.step++; }
			else if (inp.LStick.BL.on) { this->est.x = -PIXELSIZE; this->dir = Direction::LEFT; ge->system.step++; }
			else if (inp.LStick.BD.on) { this->est.y = PIXELSIZE; this->dir = Direction::DOWN; ge->system.step++; }
			else if (inp.LStick.BU.on) { this->est.y = -PIXELSIZE; this->dir = Direction::UP; ge->system.step++; }
			this->CheckBox();
		}
	}

	//Ú®æmF
	void Object::CheckMove() {
		//
		if (ge->qa_Map->CheckHit(this->pos + this->est, Player::defName)) {
			ge->system.step--;
			this->isMove = false;
			this->est = ML::Vec2(0, 0);
			return;
		}

		if (abs(this->est.x) < 0.1 && abs(this->est.y) < 0.1) {
			this->isMove = false;
			this->est = ML::Vec2(0, 0);
			return;
		}

		this->isMove = true;
		this->canTeleport = true;

		//Ú®
		switch (this->dir) {
		case Direction::LEFT:
			this->pos.x -= this->moveSpeed;
			this->est.x += this->moveSpeed;
			break;
		case Direction::RIGHT:
			this->pos.x += this->moveSpeed;
			this->est.x -= this->moveSpeed;
			break;
		case Direction::UP:
			this->pos.y -= this->moveSpeed;
			this->est.y += this->moveSpeed;
			break;
		case Direction::DOWN:
			this->pos.y += this->moveSpeed;
			this->est.y -= this->moveSpeed;
			break;
		}
	}

	//Ú®æÌ mAÚ®
	void Object::CheckBox() {
		if (ge->qa_Boxs == nullptr) { return; }

		if (ge->qa_Map->arrMap[ge->qa_Map->WATERBARRIER][(int)(this->pos.y + this->est.y) / PIXELSIZE][(int)(this->pos.x + this->est.x) / PIXELSIZE] >= 0) return;

		for (auto box = ge->qa_Boxs->begin(); box != ge->qa_Boxs->end(); ++box) {
			if ((*box)->inWater) continue;

			if ((*box)->IsNear(this->pos + this->est)) {
				if ((*box)->IsPushable(this->pos + this->est * 2))
					(*box)->AddForce(this->est, this->moveSpeed, this->dir);
				else {
					this->est = ML::Vec2(0, 0);
					ge->system.step--;
				}
				return;
			}
		}
	}

	//
	//ÈºÍî{IÉÏXsvÈ\bh
	//
	//-------------------------------------------------------------------
	//^XN¶¬û
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWÉo^
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYÉ¸sµ½çKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//\[XNXÌ¶¬
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}