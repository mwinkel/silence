/*
 * Silence
 *
 * Copyright (C) 2009 Manuel Unglaub
 *
 * This file is part of Silence.
 *
 * Silence is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version GPLv2 only of the License.
 *
 * Silence is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Silence.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "data/datastore.h"
#include "node/textnodecontent.h"
#include "node/richtextnodecontent.h"
#include <QFile>
#include <QTextStream>

DataStore::DataStore()
{
	root = new Node();
	root->setCaption("Title"); // treecaption

	labels = new QStringList;

	QDomDocument doc;
	QFile file("foo.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	doc.setContent(&file);
	file.close();
	
	QDomElement xmlRoot = doc.documentElement();
	if (xmlRoot.tagName() != "silence")
		return;
	QDomNode n = xmlRoot.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (e.tagName() == "labels")
			xmlToLabels(e);
		if (e.tagName() == "node")
			xmlToNode(root, n, doc);
		n = n.nextSibling();
	}
}

DataStore::~DataStore()
{
	delete root;
}

Node* DataStore::getRoot()
{
	return root;
}

QStringList* DataStore::getLabels()
{
	return labels;
}

void DataStore::xmlToLabels(QDomElement &xmlLabels)
{
	QDomNode n = xmlLabels.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (e.tagName() == "label")
			labels->append(e.text());
		n = n.nextSibling();
	}
}

void DataStore::xmlToNode(Node* parentNode, QDomNode &xmlNode, QDomDocument &doc)
{
	// node erstellen
	int index = parentNode->getChildCount();
	if (!parentNode->addChildren(index, 1))
		return; // TODO: richtig behandeln
	Node *node = parentNode->getChild(index);
	
	node->setId(NodeId(xmlNode.toElement().attribute("id", "-1").toInt()));

	QDateTime modificationDate = QDateTime::currentDateTime();

	QDomNode n = xmlNode.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (e.tagName() == "caption")
			node->setCaption(e.text());
		if (e.tagName() == "creationdate")
			node->setCreationDate(QDateTime::fromString(e.text()));
		if (e.tagName() == "modificationdate")
			modificationDate = QDateTime::fromString(e.text());
		if (e.tagName() == "label")
			node->addLabel(e.text());
		if (e.tagName() == "content")
		{
			if (e.attribute("mimetype", "") == "text/plain")
			{
				TextNodeContent *content = new TextNodeContent;
				content->setXmlData(e);
				node->setContent(content);
			}
			if (e.attribute("mimetype", "") == "text/richtext")
			{
				RichTextNodeContent *content = new RichTextNodeContent;
				content->setXmlData(e);
				node->setContent(content);
			}
		}

		if (e.tagName() == "node")
			xmlToNode(node, n, doc);

		n = n.nextSibling();
	}
	node->setModificationDate(modificationDate);
	
	connect(node, SIGNAL(changed(Node*)), this, SLOT(save(Node*)));
}

void DataStore::save(Node *node)
{
	// create xmlDocument
	QDomDocument doc;
	doc.setContent(QString("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"));
	QDomElement xmlRoot = doc.createElement("silence");
	doc.appendChild(xmlRoot);

	// write labels
	QDomElement xmlLabels = doc.createElement("labels");
	for (int i=0; i<labels->size(); ++i)
	{
		QDomElement l = doc.createElement("label");
		QDomText lText = doc.createTextNode(labels->at(i));
		l.appendChild(lText);
		xmlLabels.appendChild(l);
	}
	xmlRoot.appendChild(xmlLabels);

	// write nodes
	for (int i=0; i<root->getChildCount(); ++i)
	{
		addXmlNode(root->getChild(i), xmlRoot, doc);
	}
	
	// write to file
	QFile file("foo.xml");
	if( !file.open(QIODevice::WriteOnly))
		return;
	QTextStream ts(&file);
	ts << doc.toString();
	file.close();
}

void DataStore::addXmlNode(Node* node, QDomElement &parent, QDomDocument &doc)
{
	// node tag with id
	QDomElement xmlNode = doc.createElement("node");
	xmlNode.setAttribute("id", node->getId().getId());
	parent.appendChild(xmlNode);

	// caption
	QDomElement caption = doc.createElement("caption");
	QDomText captionText = doc.createTextNode(node->getCaption());
	caption.appendChild(captionText);
	xmlNode.appendChild(caption);

	// creationdate
	QDomElement cDate = doc.createElement("creationdate");
	QDomText cDateText = doc.createTextNode(node->getCreationDate().toString());
	cDate.appendChild(cDateText);
	xmlNode.appendChild(cDate);

	// modificationdate
	QDomElement mDate = doc.createElement("modificationdate");
	QDomText mDateText = doc.createTextNode(node->getModificationDate().toString());
	mDate.appendChild(mDateText);
	xmlNode.appendChild(mDate);

	// labels
	for (int i=0; i<node->getLabels()->size(); ++i)
	{
		QDomElement label = doc.createElement("label");
		QDomText labelText = doc.createTextNode(node->getLabels()->at(i));
		label.appendChild(labelText);
		xmlNode.appendChild(label);
	}

	if (node->getContent())
	{
		// content with mimetype
		QDomElement content = doc.createElement("content");
		content.setAttribute("mimetype", node->getContent()->getMimeType());
		// metainfos
		QHashIterator<QString, QString> itr(*node->getContent()->getMetaInfos());
		while (itr.hasNext())
		{
			itr.next();
			QDomElement meta = doc.createElement("metainfo");
			meta.setAttribute("key", itr.key());
			QDomText metaValue = doc.createTextNode(itr.value());
			meta.appendChild(metaValue);
			content.appendChild(meta);
		}
		// contentdata
		content.appendChild(node->getContent()->getXmlData(doc));
		xmlNode.appendChild(content);
	}

	// children
	for (int i=0; i<node->getChildCount(); ++i)
	{
		addXmlNode(node->getChild(i), xmlNode, doc);
	}
}

void DataStore::remove(Node *node)
{
	save(node);
}

void DataStore::add(Node *node)
{
	save(node);
}



