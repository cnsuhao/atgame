#include "wincore.h"
#include "dialog.h"
#include "treeview.h"
#pragma comment(lib, "comctl32.lib")
#include "resource.h"

class CView : public CWnd
{
public:
    CView() {}
    virtual ~CView() {}
    virtual void OnDestroy() { PostQuitMessage(0); }	// Ends the application
};


class CMyDialog : public CDialog
{
public:
	CMyDialog(UINT nResID):CDialog(nResID){}
	virtual ~CMyDialog(){ } 

protected:
	HTREEITEM CMyDialog::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage)
	{
		TVITEM tvi;
		ZeroMemory(&tvi, sizeof(TVITEM));
		tvi.mask = TVIF_TEXT;
		tvi.iImage = iImage;
		tvi.iSelectedImage = iImage;
		tvi.pszText = (LPTSTR)szText;

		TVINSERTSTRUCT tvis;
		ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
		tvis.hParent = hParent;
		tvis.item = tvi;

		return mView.InsertItem(tvis);
	}

	virtual BOOL OnInitDialog() 
	{
		AttachItem(IDC_TREE1,mView);

		HTREEITEM htiRoot = AddItem(NULL, _T("TreeView"), 0);
		HTREEITEM c = AddItem(htiRoot, _T("CTreeViewApp1"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		AddItem(c, _T("CTreeViewApp2"), 1);
		mView.Expand(0, 0);
		return TRUE; 
	}
	virtual void OnDestroy() { PostQuitMessage(0); }  // Ends the application

protected:
	CTreeView mView;
};

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Start Win32++
    CWinApp MyApp;
	
    // Create our view window
    CMyDialog diglog(IDD_DIALOG1);
	diglog.DoModal();

    // Run the application
    return MyApp.Run();
}