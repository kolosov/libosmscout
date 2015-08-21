/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2010  Tim Teulings

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "POICategoryModel.h"

#include <cmath>
#include <iostream>
#include <iomanip>

Category::Category(const QString& name, const QString& iconName, int id)
{
	m_name = name;
	m_id = id;
	m_iconName = iconName;
}

QString Category::name() const
{
	return m_name;
}

QString Category::iconName() const
{
	return m_iconName;
}

int Category::id() const
{
	return m_id;
}


POICategoryListModel::POICategoryListModel(QObject* parent)
{
}

POICategoryListModel::~POICategoryListModel()
{
}

QVariant POICategoryListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_categories.count())
        return QVariant();

    const Category &category = m_categories[index.row()];
    if (role == CatNameRole)
        return category.name();
    else if (role == CatIconRole)
        return category.iconName();
    else if (role == CatIDRole)
    	return category.id();
    return QVariant();
}

int POICategoryListModel::rowCount(const QModelIndex& parent) const
{
	 return m_categories.count();
}

void POICategoryListModel::addCategory(const Category& category)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_categories << category;
    endInsertRows();
}

QHash<int, QByteArray> POICategoryListModel::roleNames() const
{

    QHash<int, QByteArray> roles=QAbstractListModel::roleNames();

    roles[CatNameRole]="name";
    roles[CatIconRole]="iconName";
    roles[CatIDRole]="id";

    return roles;
}

