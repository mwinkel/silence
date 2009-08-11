#ifndef NODE_H
#define NODE_H

#include "node/abstractnodecontent.h"
#include "node/nodeid.h"
#include <QString>
#include <QStringList>
#include <QDateTime>


class Node
{
	public:
		// Konstruktoren & Destruktoren
		Node(QString caption, Node *parent = 0); 
		~Node();

		int getIndex() const;
		Node* getParent() const;
		NodeId getId() const;

		// children
//		void addChild(Node *child);
//		bool addChild(Node *child, int position);
		bool addChildren(int position, int count);
		Node* getChild(int index) const;
		int getChildCount() const;
		bool removeChildren(int position, int count);
		

		// caption
		QString getCaption() const;
		bool setCaption(QString caption);
		
		// content
		AbstractNodeContent* getContent() const;
		void setContent(AbstractNodeContent *content);

		// dates
		QDateTime getCreationDate() const;
		QDateTime getModificationDate() const;
		void setModificationDate(QDateTime &date);

		int columnCount() const;

		// labels
		QStringList getLabels() const;
		void addLabel(QString label);
		
	private:
		NodeId id;
		QString caption;
		AbstractNodeContent *content;
		QDateTime creationDate;
		QDateTime modificationDate;
		QStringList labels;
/*
		icon
		QString mimeType;
*/		
		Node *parent;
		QList<Node*> children;
};

#endif // NODE_H
