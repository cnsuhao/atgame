#include "StdAfx.h"
#include "ViewerApp.h"
#include "atgRenderer.h"
#include "atgCamera.h"
#include "atgLight.h"


#include "atgMaterial.h"
#include "atgShaderLibrary.h"

#include "FxbModel.h"

#include "resource.h"


class MyProperty
{
public : 
    MyProperty()
    {
        m_propertys.insert(std::pair<std::string, std::string>("name", "lsh"));
    }

    const std::string& operator [](const std::string& key)
    {
        static std::string emptyStr("");
        std::map<std::string, std::string>::iterator it = m_propertys.find(key);
        if (it != m_propertys.end())
        {
            return it->second;
        }
        return emptyStr;
    }


    std::map<std::string, std::string> m_propertys;
};

ViewerApp::ViewerApp(void)
{
	pModel = NULL;
	pCamera = NULL;
	pLight1 = NULL;
    pLight2 = NULL;

	leftBtnDown = false;
	rightBtnDown = false;
	middleBtnDown = false;
}


ViewerApp::~ViewerApp(void)
{
}

bool ViewerApp::OnInitialize()
{
    MyProperty p1;
    LOG("Property[%s]=[%s]", "name", p1["name"].c_str());

	g_Renderer->SetViewport(0, 0, GetWindowWidth(), GetWindowHeight());
	g_Renderer->SetFaceCull(FCM_CW);

	pCamera = new atgCamera();
	g_Renderer->SetMatrix(MD_VIEW, pCamera->GetView().m);

    g_Renderer->SetGlobalAmbientColor(Vec3(0.2f,0.2f,0.2f));

    //atgDirectionalLight* pDirectionalLight = new atgDirectionalLight();
    //pDirectionalLight->SetDirection(Vec3(1.0f, -1.0f, 0.0f));
    //pDirectionalLight->SetColor(Vec3(0.3f, 0.3f, 0.3f));
    //pDirectionalLight->SetSpecular(Vec3One);
    //pDirectionalLight->SetIntensity(1.0f);
    ////pLight = pDirectionalLight;
    //g_Renderer->AddBindLight(pDirectionalLight);

    atgPointLight* pPointLight = new atgPointLight();
    pPointLight->SetPosition(Vec3(30,80,0));
    pPointLight->SetColor(Vec3(1.0f, 1.0f, 1.0f));
    pPointLight->SetSpecular(Vec3One);
    pPointLight->SetRange(50.0f);
    pLight1 = pPointLight;
    g_Renderer->AddBindLight(pLight1);

    atgSpotLight* pSpotLight = new atgSpotLight();
    pSpotLight->SetPosition(Vec3(0,80,0));
    pSpotLight->SetColor(Vec3(1.0f, 0.0f, 1.0f));
    pSpotLight->SetSpecular(Vec3One);
    pSpotLight->SetRange(200.0f);
    pSpotLight->SetOuterCone(35.0f);
    pSpotLight->SetInnerCone(30.0f);
    pSpotLight->SetDirection(Vec3(0.0f, -1.0f, 0.0));
    pLight2 = pSpotLight;
    g_Renderer->AddBindLight(pLight2);

    RECT w_rect;
    GetClientRect(g_hWnd, &w_rect);
    float aspect = (w_rect.right - w_rect.left) * 1.0f / (w_rect.bottom - w_rect.top) * 1.0f;
    pCamera->SetProjection(60.0f, aspect, 10.0f, 500.0f);
    g_Renderer->SetMatrix(MD_PROJECTION, pCamera->GetProj().m);

    g_Renderer->SetCamera(pCamera);

	pModel = new FxbModel("model\\modelBuilding_House1.fbx");

    SetTitle("<fbxViewer.>");

	return true;
}

void ViewerApp::OnShutdown()
{
	SAFE_DELETE (pModel);
	SAFE_DELETE (pCamera);
    SAFE_DELETE (pLight1);
    SAFE_DELETE (pLight2);
}

void ViewerApp::OnPause()
{

}

void ViewerApp::OnResume()
{

}

void ViewerApp::OnFrame()
{
	g_Renderer->Clear();

	g_Renderer->BeginFrame();


    //static Vec3 p;
    //p = pLight->GetPosition();
    //Quat q;
    //Vec3 v(0.0f, 1.0f, 0.0f);
    //QuatFromAxisAngle(q.m, Vec3(0.0f, 1.0f, 0.0f).m, 0.1f);
    //VecRotate(p.m, p.m, q.m);
    //pLight->SetPosition(p);
    //static float intensity = 0.0f;
    //intensity += 0.01f;
    //pLight->SetIntensity(1.0f + fmodf(intensity, 4.0f));
    
    //Vec3 p; 
    //float radius = 66.23f;
    //static float a = 0.0f;
    //a += 1.0f;
    //if (!(a < 360.0f))
    //    a = 0.0f;
    //float b = 0.326f*a;
    //p.x = radius * sinf(DegreesToRadians(a)) * cosf(DegreesToRadians(b));
    //p.y = radius * sinf(DegreesToRadians(a)) * sinf(DegreesToRadians(b));
    //p.y = radius * cosf(DegreesToRadians(a));
    //pLight->SetPosition(p);

    //static float t = 0.0f;
    //t += 0.01f;
    //if (!(t < 360.0f))
    //    t = 0.0f;
    //Vec3 pos;
    //pos.x = 50.0f * cosf(1.24f * t) * sinf(3.12f * t);
    //pos.y = 50.0f * sinf(2.97f * t) * cosf(0.81f * t);
    //pos.z = 50.0f * cosf(t) * sinf(t * 1.231f);
    //pLight->SetPosition(pos);


	g_Renderer->SetMatrix(MD_VIEW, pCamera->GetView().m);
	g_Renderer->SetMatrix(MD_PROJECTION, pCamera->GetProj().m);
    static float t = 0.0f;
    t += 0.1f;
    if (!(t < 100.0f))
        t = -100.0f;
    //Vec3 pos = pLight->GetPosition();
    //pos.x = t;
    //pLight->SetPosition(pos);
    //pLight->SetPosition(pCamera->GetPosition().m);

	pModel->Render();

    const bindLights& lights = g_Renderer->GetBindLights();
    bindLights::const_iterator it = lights.begin();
    for (bindLights::const_iterator end = lights.end(); it != end; ++it)
    {
        (*it)->DebugDraw();
    }

	g_Renderer->EndFrame();

	g_Renderer->Present();
}

void ViewerApp::OnPointerDown( uint8 id, int16 x, int16 y )
{
	if (MBID_LEFT == id)
	{
		leftBtnDown = true;
	}
	else if (MBID_RIGHT == id)
	{
		rightBtnDown = true;
	}
	else if (MBID_MIDDLE == id)
	{
		middleBtnDown = true;
	}
}

void ViewerApp::OnPointerMove( uint8 id, int16 x, int16 y )
{
	static uint16 last_x = x;
	static uint16 last_y = y;

	static float oYaw = DegreesToRadians(180.f);
	static float oPitch = DegreesToRadians(45.f);

	if (id == MBID_MIDDLE)
	{
		float moveSpeed = 1.5f;
		if (x)
		{
			Vec3 forward = pCamera->GetForward();
			Vec3 pos = pCamera->GetPosition();
			forward.Scale(moveSpeed);
			pos.Add(forward.m);
			pCamera->SetPosition(pos.m);
		}
		else
		{
			Vec3 forward = pCamera->GetForward();
			Vec3 pos = pCamera->GetPosition();
			forward.Scale(-moveSpeed);
			pos.Add(forward.m);
			pCamera->SetPosition(pos.m);
		}
		return;
	}

	if (rightBtnDown)
	{
		float dx = static_cast<float>(x - last_x);
		float dy = static_cast<float>(y - last_y);

		if(pCamera)
		{
			oYaw -= dx * 0.001f;
			oPitch += dy * 0.001f;
            oPitch = Clamp(oPitch, DegreesToRadians(-90.0f), DegreesToRadians(90.0f));
			pCamera->SetYaw(oYaw);
			pCamera->SetPitch(oPitch);
		}
	}
	last_x = x;
	last_y = y;
}

void ViewerApp::OnPointerUp( uint8 id, int16 x, int16 y )
{
	if (MBID_LEFT == id)
	{
		leftBtnDown = false;
	}
	else if (MBID_RIGHT == id)
	{
		rightBtnDown = false;
	}
	else if (MBID_MIDDLE == id)
	{
		middleBtnDown = false;
	}
}


#ifdef _WIN32

#include <commctrl.h>

HWND g_control_dialog = NULL;

BOOL CALLBACK Control_Dialog_Proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    //BOOL fError; 

    switch (message) 
    { 
    case WM_INITDIALOG: 
        //CheckDlgButton(hwndDlg, ID_ABSREL, fRelative); 
        return TRUE; 

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDOK: 
            //fRelative = IsDlgButtonChecked(hwndDlg, 
            //    ID_ABSREL); 
            //iLine = GetDlgItemInt(hwndDlg, ID_LINE, 
            //    &fError, fRelative); 
            //if (fError) 
            //{ 
            //    MessageBox(hwndDlg, SZINVALIDNUMBER, 
            //        SZGOTOERR, MB_OK); 
            //    SendDlgItemMessage(hwndDlg, ID_LINE, 
            //        EM_SETSEL, 0, -1L); 
            //} 
            //else 

            //    // Notify the owner window to carry out the task. 

                return TRUE; 

        case IDCANCEL: 
            DestroyWindow(g_control_dialog); 
            g_control_dialog = NULL; 
            return TRUE; 
        } 
    } 
    return FALSE; 
} 
#endif // _WIN32

void ViewerApp::OnKeyScanDown( Key::Scan keyscan )
{
	float moveSpeed = 1.5f;

	switch (keyscan)
	{
	case Key::W:
		{
		    Vec3 forward = pCamera->GetForward();
			Vec3 pos = pCamera->GetPosition();
		    forward.Scale(moveSpeed);
			pos.Add(forward.m);
			pCamera->SetPosition(pos.m);

		}
		break;
	case Key::S:
		{
			Vec3 forward = pCamera->GetForward();
			Vec3 pos = pCamera->GetPosition();
			forward.Scale(-moveSpeed);
			pos.Add(forward.m);
			pCamera->SetPosition(pos.m);
		}
		break;
	case Key::A:
		{
			Vec3 right = pCamera->GetRight();
			Vec3 pos = pCamera->GetPosition();
			right.Scale(moveSpeed);
			pos.Add(right.m);
			pCamera->SetPosition(pos.m);
		}
		break;
	case Key::D:
		{
			Vec3 right = pCamera->GetRight();
			Vec3 pos = pCamera->GetPosition();
			right.Scale(-moveSpeed);
			pos.Add(right.m);
			pCamera->SetPosition(pos.m);
		}
		break;
    case Key::P:
        {
            static int lightType = 0;
            lightType = ++lightType % 2;
            //pLight->SetType((LightType)lightType);
#ifdef _WIN32

            if (!g_control_dialog)
            {
                g_control_dialog = CreateDialog(g_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),g_hWnd, &Control_Dialog_Proc);
                RECT rect;
                ::GetWindowRect(g_hWnd, &rect);

                ::SetWindowPos(g_control_dialog, NULL,rect.right, rect.top,0,0,SWP_NOSIZE | SWP_SHOWWINDOW);

                //::ShowWindow(g_control_dialog, SW_SHOWDEFAULT);
                HWND g_tree = ::GetDlgItem(g_control_dialog, IDC_TREE1);
                if (g_tree)
                {
                    TVINSERTSTRUCT tvins;
                    TVITEM item;
                    item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_SELECTEDIMAGE;
                    item.pszText = L"1111";
                    item.cchTextMax = sizeof(item.pszText)/sizeof(item.pszText[0]);
                    item.iImage = 1;
                    item.iSelectedImage = 2;
                    item.cChildren = I_CHILDRENCALLBACK;
                    tvins.item = item;
                    tvins.hInsertAfter = TVI_SORT;
                    tvins.hParent = TVI_ROOT;
                    TreeView_InsertItem(g_tree,&tvins);
                    //::SendMessage(g_tree, 0,0,0);
                }
            }
#endif // _WIN32
        }
        break;
    case Key::N:
        {
            Vec3 p = static_cast<atgSpotLight*>(pLight1)->GetPosition();
            p.x += 1;
            static_cast<atgSpotLight*>(pLight1)->SetPosition(p);
        }
        break;
    case Key::M:
        {
            Vec3 p = static_cast<atgSpotLight*>(pLight1)->GetPosition();
            p.x -= 1;
            static_cast<atgSpotLight*>(pLight1)->SetPosition(p);
        }
        break;
    case Key::H:
        {
            float r = static_cast<atgSpotLight*>(pLight1)->GetRange();
            r += 1;
            static_cast<atgSpotLight*>(pLight1)->SetRange(r);
        }
        break;
    case Key::J:
        {
            float r = static_cast<atgSpotLight*>(pLight1)->GetRange();
            r -= 1;
            static_cast<atgSpotLight*>(pLight1)->SetRange(r);
        }
        break;
    case Key::C:
        {
            float r = static_cast<atgSpotLight*>(pLight1)->GetIntensity();
            r += 1;
            static_cast<atgSpotLight*>(pLight1)->SetIntensity(r);
        }
        break;
    case Key::V:
        {
            float r = static_cast<atgSpotLight*>(pLight1)->GetIntensity();
            r -= 1;
            static_cast<atgSpotLight*>(pLight1)->SetIntensity(r);
        }
        break;
	default:
		break;
	}

	atgGame::OnKeyScanDown(keyscan);
}

void ViewerApp::OnKeyScanUp( Key::Scan keyscan )
{
	atgGame::OnKeyScanUp(keyscan);
}
