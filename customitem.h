// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

//! [1]
#include <QtQuick>

class CustomItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit CustomItem(QQuickItem *parent = nullptr);
    virtual ~CustomItem();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private slots:
    void onWindowChanged(QQuickWindow* window);
    void onSceneGraphInitialized();

private:
    QSGTexture* _texture;
    bool _initialized;
};
//! [1]
#endif // CUSTOMITEM_H
