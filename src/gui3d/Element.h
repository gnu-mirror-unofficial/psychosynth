/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

#include "gui3d/FlatRing.h"
#include "gui3d/ElementProperties.h"
#include "table/Table.h"

class Element;

class ElemComponent
{
    Element* m_parent;
    Ogre::SceneNode* m_node;
    
public:    
    virtual ~ElemComponent() {};
    virtual void init() = 0;
    virtual void handleParamChange(TableObject& obj, int param_id) = 0;
    virtual bool handlePointerMove(Ogre::Vector2 pos) = 0;
    virtual bool handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    virtual bool handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    
    void setSceneNode(Ogre::SceneNode* node) {
	m_node = node;
    }

    inline void updateVisibility();
    
    Ogre::SceneNode* getSceneNode() {
	return m_node;
    }

    void setParent(Element* parent) {
	m_parent = parent;
    }
    
    Element* getParent() {
	return m_parent;
    };
};

class Element : public TableObjectListener
{
    typedef std::list<ElemComponent*>::iterator ElemComponentIter;
    std::list<ElemComponent*> m_comp;

    /*
    std::list<Connection*> m_src_con;
    std::list<Connection*> m_dest_con;
    */
    
    TableObject m_obj;
    TableObject m_target;
    
    Ogre::ColourValue   m_col_ghost;
    Ogre::ColourValue   m_col_selected;
    Ogre::ColourValue   m_col_normal;
    Ogre::SceneManager* m_scene;
    FlatRing*           m_base;
    Ogre::SceneNode*    m_node;
    
    Ogre::Vector3 m_aimpoint;
    Ogre::Vector2 m_click_diff;
    Ogre::Vector2 m_pos;
	
    bool m_ghost;
    bool m_selected;
    bool m_moving;

    ElementProperties m_gui_prop;
public:
    static const Real RADIOUS = 1.0f;
    static const Real Z_POS = 0.001f;
    
    Element(const TableObject& obj, Ogre::SceneManager* scene);
    
    virtual ~Element();

    void addComponent(ElemComponent* comp);

    void setTarget(const TableObject& obj);
    void clearTarget(const TableObject& obj);
    void setGhost(bool ghost);
    void setSelected(bool selected);
    void setPosition(const Ogre::Vector2& pos);
    
    bool pointerClicked(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerMoved(const Ogre::Vector2& pos);
    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);
    
    void handleMoveObject(TableObject& obj);
    void handleActivateObject(TableObject& obj);
    void handleDeactivateObject(TableObject& obj);
    void handleSetParamObject(TableObject& ob, int param_id);
    
    bool isGhost() const {
	return m_ghost;
    };
	
    bool isSelected() const {
	return m_selected;
    };
	
    Ogre::Vector2 getPosition() {
	return m_pos;
    }

    Ogre::SceneManager* getScene() {
	return m_scene;
    }

    TableObject& getObject() {
	return m_obj;
    }

    ElementProperties& getGUIProperties() {
	return m_gui_prop;
    }
    /*
    void addSourceConnection(Connection* con) {
	m_src_con.push_back(con);
    };

    void addDestinyConnection(Connection* con) {
	m_dest_con.push_back(con);
    };

    void removeSourceConnection(Connection* con) {
	m_src_con.remove(con);
    }

    void removeDestinyConnection(Connection* con) {
	m_dest_con.remove(con);
    }
    */
};

inline void ElemComponent::updateVisibility()
{
    m_node->setVisible(!m_parent->isGhost());
}

#endif /* ELEMENT_H */