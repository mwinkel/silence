#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "data/datastore.h"
#include "gui/contentview.h"
#include "gui/editmenu.h"
#include "gui/infosidebar.h"
#include "gui/nodepropertywidget.h"
#include "gui/statusbar.h"
#include "gui/treeview.h"


class Controller
{
	public:
		~Controller();
		static Controller* create();

		void setContentView(ContentView* contentview);
		ContentView* getContentView() const;

		void setInfoSidebar(InfoSidebar* infosidebar);
		InfoSidebar* getInfoSidebar() const;
		
		void setTreeView(TreeView* treeview);
		TreeView* getTreeView() const;
		
		void setStatusBar(StatusBar* statusbar);
		StatusBar* getStatusBar() const;

		void setNodePropertyWidget(NodePropertyWidget *nodepropertywidget);
		NodePropertyWidget* getNodePropertyWidget() const;

		void setEditMenu(EditMenu *editmenu);
		EditMenu* getEditMenu() const;

		DataStore* getDataStore();

	protected:
		Controller();

	private:
		static Controller *controller;
		
		ContentView *contentview;
		InfoSidebar *infosidebar;
		TreeView *treeview;
		StatusBar *statusbar;
		NodePropertyWidget *nodepropertywidget;
		EditMenu *editmenu;

		DataStore *datastore;
};

#endif // CONTROLLER_H

