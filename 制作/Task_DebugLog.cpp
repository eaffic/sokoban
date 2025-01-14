//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_DebugLog.h"

namespace  DebugLog
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		this->font = DG::Font::Create("MSSVbN", 12, 16);
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		this->font.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//uϊ»v^XNΆ¬ΙPρΎ―s€
	bool  Object::Initialize()
	{
		//X[p[NXϊ»
		__super::Initialize(defGroupName, defName, true);
		//\[XNXΆ¬or\[X€L
		this->res = Resource::Create();

		//f[^ϊ»
		this->render2D_Priority[1] = 0.1f;
		this->message = "Debug Message";

		//^XNΜΆ¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
		this->ResetMessage();
	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		if (!ge->system.debug) return;

		ML::Box2D textBox(400, 500, 800, 300);
		this->res->font->Draw(textBox, message, ML::Color(1, 1, 1, 1));
		this->message.clear();

		if (ge->qa_Boxs != nullptr) {
			for (auto it = ge->qa_Boxs->begin(); it != ge->qa_Boxs->end(); ++it) {
				textBox = ML::Box2D(-32, -16, 64, 48);
				textBox.Offset((*it)->pos.x, (*it)->pos.y);
				textBox.Offset(-ge->camera2D.x, -ge->camera2D.y);
				this->res->font->Draw(textBox, "    " + to_string((*it)->number) + "\n" + to_string((*it)->render2D_Priority[1]), ML::Color(1, 1, 0, 0));
			}
		}

		ge->fps.UpdateFPS();	
		ge->fps.DrawFps();
	}

	void Object::DebugPrint(string str_) {
		this->message += "\n" + str_;
	}

	void Object::ResetMessage() {
		this->message.clear();
	}

	//
	//ΘΊΝξ{IΙΟXsvΘ\bh
	//
	//-------------------------------------------------------------------
	//^XNΆ¬ϋ
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWΙo^
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYΙΈs΅½ηKill
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
	//\[XNXΜΆ¬
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