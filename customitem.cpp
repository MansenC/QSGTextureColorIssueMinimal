// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "customitem.h"

#include <QtCore/QPointer>

#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGTexture>
#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGTextureProvider>

//! [2]
class CustomShader : public QSGMaterialShader
{
public:
    CustomShader()
    {
        setShaderFileName(VertexStage, QLatin1String(":/shaders/textured.vert.qsb"));
        setShaderFileName(FragmentStage, QLatin1String(":/shaders/textured.frag.qsb"));
    }
    bool updateUniformData(RenderState &state,
                           QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
    void updateSampledImage(RenderState& state, int binding, QSGTexture** texture, QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override;
};
//! [2]

//! [1]
class CustomMaterial : public QSGMaterial
{
public:
    CustomMaterial(QSGTexture* texture = nullptr);
    QSGMaterialType *type() const override;
    int compare(const QSGMaterial *other) const override;

    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode) const override
    {
        return new CustomShader;
    }

    void setTexture(QSGTexture* texture);
    QSGTexture* getTexture() const;

    struct {
        float color[4];
    } uniforms;

protected:
    QSGTexture* _texture;
};
//! [1]

CustomMaterial::CustomMaterial(QSGTexture* texture)
    : QSGMaterial()
{
    setTexture(texture);
}

QSGMaterialType *CustomMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

int CustomMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const auto *other = static_cast<const CustomMaterial *>(o);
    return other == this ? 0 : 1; // ### TODO: compare state???
}

void CustomMaterial::setTexture(QSGTexture* texture)
{
    _texture = texture;
    setFlag(Blending, _texture && _texture->hasAlphaChannel());
}

QSGTexture* CustomMaterial::getTexture() const
{
    return _texture;
}

//! [3]
bool CustomShader::updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
{
    bool changed = false;
    QByteArray *buf = state.uniformData();

    if (state.isMatrixDirty()) {
        const QMatrix4x4 m = state.combinedMatrix();
        memcpy(buf->data(), m.constData(), 64);
        changed = true;
    }

    if (state.isOpacityDirty()) {
        const float opacity = state.opacity();
        memcpy(buf->data() + 64, &opacity, 4);
        changed = true;
    }

    float color[4] = { 1, 1, 1, 1 };
    memcpy(buf->data() + 68, color, sizeof(color));

    return changed;
}
//! [3]

void CustomShader::updateSampledImage(
    RenderState& state,
    int binding,
    QSGTexture** texture,
    QSGMaterial* newMaterial,
    QSGMaterial*)
{
    if (binding != 1)
    {
        return;
    }

    CustomMaterial* material = static_cast<CustomMaterial*>(newMaterial);
    QSGTexture* newTexture = material->getTexture();
    if (newTexture == nullptr)
    {
        *texture = nullptr;
        return;
    }

    newTexture->commitTextureOperations(state.rhi(), state.resourceUpdateBatch());
    *texture = newTexture;
}

//! [4]
class CustomNode : public QSGGeometryNode
{
public:
    CustomNode(QSGTexture* texture = nullptr)
    {
        auto *m = new CustomMaterial(texture);
        setMaterial(m);
        setFlag(OwnsMaterial, true);

        QSGGeometry *g = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
        QSGGeometry::updateTexturedRectGeometry(g, QRect(), QRect());
        setGeometry(g);
        setFlag(OwnsGeometry, true);
    }

    void setRect(const QRectF &bounds)
    {
        QSGGeometry::updateTexturedRectGeometry(geometry(), bounds, QRectF(0, 0, 1, 1));
        markDirty(QSGNode::DirtyGeometry);
    }
};
//! [4]

CustomItem::CustomItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    _initialized = false;
    setFlag(ItemHasContents, true);

    connect(
        this,
        &QQuickItem::windowChanged,
        this,
        &CustomItem::onWindowChanged);
}

CustomItem::~CustomItem()
{
}

void CustomItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    update();
    QQuickItem::geometryChange(newGeometry, oldGeometry);
}

void CustomItem::onWindowChanged(QQuickWindow* window)
{
    if (!window)
    {
        _texture->deleteLater();
        return;
    }

    connect(
        window,
        &QQuickWindow::sceneGraphInitialized,
        this,
        &CustomItem::onSceneGraphInitialized);
}

void CustomItem::onSceneGraphInitialized()
{
    if (!_initialized)
    {
        QImage image(":/bob.jpg");
        _texture = window()->createTextureFromImage(image);
        _initialized = true;
    }
}

//! [6]
QSGNode *CustomItem::updatePaintNode(QSGNode *old, UpdatePaintNodeData *)
{
    if (!_initialized)
    {
        QImage image(":/bob.jpg");
        _texture = window()->createTextureFromImage(image);
        _initialized = true;
    }

    auto *node = static_cast<CustomNode *>(old);

    if (!node)
        node = new CustomNode(_texture);

    node->setRect(boundingRect());
    return node;
}
//! [6]
