/*
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of Open-Sankoré.
 *
 * Open-Sankoré is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * Open-Sankoré is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open-Sankoré.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef UBGRAPHICSSCENE_H_
#define UBGRAPHICSSCENE_H_

#include <QtGui>

#include "frameworks/UBCoreGraphicsScene.h"

#include "core/UB.h"

#include "UBItem.h"
#include "tools/UBGraphicsCurtainItem.h"
#include "board/UBFeaturesController.h"

class UBGraphicsPixmapItem;
class UBGraphicsProxyWidget;
class UBGraphicsSvgItem;
class UBGraphicsPolygonItem;
class UBGraphicsMediaItem;
class UBGraphicsVideoItem;
class UBGraphicsAudioItem;
class UBGraphicsWidgetItem;
class UBGraphicsW3CWidgetItem;
class UBGraphicsAppleWidgetItem;
class UBToolWidget;
class UBGraphicsPDFItem;
class UBGraphicsTextItem;
class UBGraphicsRuler;
class UBGraphicsProtractor;
class UBGraphicsCompass;
class UBDocumentProxy;
class UBGraphicsCurtainItem;
class UBGraphicsStroke;
class UBMagnifierParams;
class UBMagnifier;
class UBGraphicsCache;
class UBGraphicsGroupContainerItem;

const double PI = 4.0 * atan(1.0);

class UBZLayerController : public QObject
{
    Q_OBJECT

public:
    struct ItemLayerTypeData {
        ItemLayerTypeData() : bottomLimit(0), topLimit(0), curValue(0), incStep(1) {;}
        ItemLayerTypeData(qreal bot, qreal top, qreal increment = 1) : bottomLimit(bot), topLimit(top), curValue(bot), incStep(increment) {;}
        qreal bottomLimit; //bottom bound of the layer
        qreal topLimit;//top bound of the layer
        qreal curValue;//current value of variable
        qreal incStep;//incremental step
    };

    enum moveDestination {
        up
        , down
        , top
        , bottom
    };

    typedef QMap<itemLayerType::Enum, ItemLayerTypeData> ScopeMap;

    UBZLayerController(QGraphicsScene *scene);

    qreal getBottomLimit(itemLayerType::Enum key) const {return scopeMap.value(key).bottomLimit;}
    qreal getTopLimit(itemLayerType::Enum key) const {return scopeMap.value(key).topLimit;}
    bool validLayerType(itemLayerType::Enum key) const {return scopeMap.contains(key);}

    static qreal errorNum() {return errorNumber;}

    qreal generateZLevel(itemLayerType::Enum key);
    qreal generateZLevel(QGraphicsItem *item);

    qreal changeZLevelTo(QGraphicsItem *item, moveDestination dest);
    itemLayerType::Enum typeForData(QGraphicsItem *item) const;
    void setLayerType(QGraphicsItem *pItem, itemLayerType::Enum pNewType);

private:
    ScopeMap scopeMap;
    static qreal errorNumber;
    QGraphicsScene *mScene;
};

class UBGraphicsScene: public UBCoreGraphicsScene, public UBItem
{
    Q_OBJECT

    public:

    enum clearCase {
        clearItemsAndAnnotations = 0
        , clearAnnotations
        , clearItems
        , clearBackground
    };

    //        tmp stub for divide addings scene objects from undo mechanism implementation
        void enableUndoRedoStack(){mUndoRedoStackEnabled = true;}
        void setURStackEnable(bool enable){mUndoRedoStackEnabled = enable;}
        bool isURStackIsEnabled(){return mUndoRedoStackEnabled;}

        UBGraphicsScene(UBDocumentProxy *parent, bool enableUndoRedoStack = true);
        virtual ~UBGraphicsScene();

        virtual UBItem* deepCopy() const;

        virtual void copyItemParameters(UBItem *copy) const {Q_UNUSED(copy);}

        UBGraphicsScene* sceneDeepCopy() const;

        void clearContent(clearCase pCase = clearItemsAndAnnotations);

        bool inputDevicePress(const QPointF& scenePos, const qreal& pressure = 1.0);
        bool inputDeviceMove(const QPointF& scenePos, const qreal& pressure = 1.0);
        bool inputDeviceRelease();

        void leaveEvent (QEvent* event);

        void addItem(QGraphicsItem* item);
        void removeItem(QGraphicsItem* item);

        void addItems(const QSet<QGraphicsItem*>& item);
        void removeItems(const QSet<QGraphicsItem*>& item);

        void addShapeToUndoStack(QGraphicsItem* item);
        void removeShapeToUndoStack(QGraphicsItem* item);

        UBGraphicsWidgetItem* addWidget(const QUrl& pWidgetUrl, const QPointF& pPos = QPointF(0, 0));
        UBGraphicsAppleWidgetItem* addAppleWidget(const QUrl& pWidgetUrl, const QPointF& pPos = QPointF(0, 0));
        UBGraphicsW3CWidgetItem* addW3CWidget(const QUrl& pWidgetUrl, const QPointF& pPos = QPointF(0, 0));
        void addGraphicsWidget(UBGraphicsWidgetItem* graphicsWidget, const QPointF& pPos = QPointF(0, 0));

        // Issue 1598/1605 - CFA - 20131028
        QPointF lastCenter();
        void setLastCenter(QPointF center);
        // Fin Issue 1598/1605 - CFA - 20131028

        UBGraphicsMediaItem* addMedia(const QUrl& pMediaFileUrl, bool shouldPlayAsap, const QPointF& pPos = QPointF(0, 0));
        UBGraphicsMediaItem* addVideo(const QUrl& pVideoFileUrl, bool shouldPlayAsap, const QPointF& pPos = QPointF(0, 0));
        UBGraphicsMediaItem* addAudio(const QUrl& pAudioFileUrl, bool shouldPlayAsap, const QPointF& pPos = QPointF(0, 0));
        UBGraphicsSvgItem* addSvg(const QUrl& pSvgFileUrl, const QPointF& pPos = QPointF(0, 0), const QByteArray pData = QByteArray());
        UBGraphicsTextItem* addText(const QString& pString, const QPointF& pTopLeft = QPointF(0, 0));
        UBGraphicsTextItem* textForObjectName(const QString& pString, const QString &objectName = "UBTGZeroPageSessionTitle");

        UBGraphicsTextItem*  addTextWithFont(const QString& pString, const QPointF& pTopLeft = QPointF(0, 0)
                , int pointSize = -1, const QString& fontFamily = "", bool bold = false, bool italic = false);
        UBGraphicsTextItem* addTextHtml(const QString &pString = QString(), const QPointF& pTopLeft = QPointF(0, 0));

        UBGraphicsW3CWidgetItem* addOEmbed(const QUrl& pContentUrl, const QPointF& pPos = QPointF(0, 0));

        UBGraphicsGroupContainerItem *createGroup(QList<QGraphicsItem*> items);
        void addGroup(UBGraphicsGroupContainerItem *groupItem);

        QGraphicsItem* setAsBackgroundObject(QGraphicsItem* item, bool pAdaptTransformation = false, bool expand = false, UBFeatureBackgroundDisposition disposition = Center);

        QUrl backgroundObjectUrl();
        void setBackgroundObjectUrl(QUrl url);

        const UBFeatureBackgroundDisposition backgroundObjectDisposition() const
        {
            return mBackgroundObjectDisposition;
        }

        QGraphicsItem* backgroundObject() const
        {
            return mBackgroundObject;
        }

        bool isBackgroundObject(const QGraphicsItem* item) const
        {
            return item == mBackgroundObject;
        }

        QGraphicsItem* scaleToFitDocumentSize(QGraphicsItem* item, bool center = false, int margin = 0, bool expand = false, UBFeatureBackgroundDisposition disposition = Center);

        QRectF normalizedSceneRect(qreal ratio = -1.0);

        QGraphicsItem *itemForUuid(QUuid uuid);

        void moveTo(const QPointF& pPoint);
        void drawLineTo(const QPointF& pEndPoint, const qreal& pWidth, bool bLineStyle);
        void eraseLineTo(const QPointF& pEndPoint, const qreal& pWidth);
        void drawArcTo(const QPointF& pCenterPoint, qreal pSpanAngle);

        bool isEmpty() const;

        //issue 1554 - NNE - 20131010
        void deselectAllItemsExcept(QGraphicsItem* gti);

        void setDocument(UBDocumentProxy* pDocument);

        UBDocumentProxy* document() const
        {
            return mDocument;
        }

        bool isDarkBackground() const
        {
            return mDarkBackground;
        }

        bool isLightBackground() const
        {
            return !mDarkBackground;
        }

        bool isCrossedBackground() const
        {
            return mCrossedBackground;
        }

        bool hasBackground()
        {
            return (mBackgroundObject != 0);
        }

        void addRuler(QPointF center);
        void addProtractor(QPointF center);
        void addCompass(QPointF center);
        void addTriangle(QPointF center);
        void addMagnifier(UBMagnifierParams params);
        void addAristo(QPointF center);

        void addMask(const QPointF &center = QPointF());
        void addCache();

        QList<QGraphicsItem*> getFastAccessItems()
        {
            return mFastAccessItems;
        }

        class SceneViewState
        {
            QPointF mLastSceneCenter;// Issue 1598/1605 - CFA - 20131028

            public:
                SceneViewState()
                {
                    zoomFactor = 1;
                    horizontalPosition = 0;
                    verticalPostition = 0;
                    mLastSceneCenter = QPointF();// Issue 1598/1605 - CFA - 20131028
                }

                SceneViewState(qreal pZoomFactor, int pHorizontalPosition, int pVerticalPostition, QPointF sceneCenter)// Issue 1598/1605 - CFA - 20131028
                {
                    zoomFactor = pZoomFactor;
                    horizontalPosition = pHorizontalPosition;
                    verticalPostition = pVerticalPostition;
                    mLastSceneCenter = sceneCenter;// Issue 1598/1605 - CFA - 20131028
                }                                

                // Issue 1598/1605 - CFA - 20131028
                QPointF lastSceneCenter() // Save Scene Center to replace the view when the scene becomes active
                {
                    return mLastSceneCenter;
                }

                void setLastSceneCenter(QPointF center)
                {
                    mLastSceneCenter = center;
                }
                // Fin issue 1598/1605 - CFA - 20131028

                qreal zoomFactor;
                int horizontalPosition;
                int verticalPostition;

        };

        SceneViewState viewState() const
        {
            return mViewState;
        }

        void setViewState(const SceneViewState& pViewState)
        {
            mViewState = pViewState;
        }

        virtual void setRenderingQuality(UBItem::RenderingQuality pRenderingQuality);

        QList<QUrl> relativeDependencies() const;

        QSize nominalSize();

        void setNominalSize(const QSize& pSize);

        void setNominalSize(int pWidth, int pHeight);

        qreal changeZLevelTo(QGraphicsItem *item, UBZLayerController::moveDestination dest);

        enum RenderingContext
        {
            Screen = 0, NonScreen, PdfExport, Podcast
        };

        void setRenderingContext(RenderingContext pRenderingContext)
        {
            mRenderingContext = pRenderingContext;
        }

        RenderingContext renderingContext() const
        {
            return mRenderingContext;
        }

        QSet<QGraphicsItem*> tools(){ return mTools;}

        void registerTool(QGraphicsItem* item)
        {
            mTools << item;
        }

        const QPointF& previousPoint()
        {
            return mPreviousPoint;
        }

        void setSelectedZLevel(QGraphicsItem *item);
        void setOwnZlevel(QGraphicsItem *item);

        static QUuid getPersonalUuid(QGraphicsItem *item);

        UBGraphicsPolygonItem* polygonToPolygonItem(const QPolygonF pPolygon);

public slots:
		void initStroke();
        void hideEraser();

        void setBackground(bool pIsDark, bool pIsCrossed);
        void setBackgroundZoomFactor(qreal zoom);
        void setDrawingMode(bool bModeDesktop);
        void deselectAllItems();

        UBGraphicsPixmapItem* addPixmap(const QPixmap& pPixmap, 
            QGraphicsItem* replaceFor,
            const QPointF& pPos = QPointF(0,0), 
            qreal scaleFactor = 1.0, 
            bool pUseAnimation = false);

        void textUndoCommandAdded(UBGraphicsTextItem *textItem);

        void setToolCursor(int tool);

        void selectionChangedProcessing();
        void updateGroupButtonState();
        void moveMagnifier();
        void moveMagnifier(QPoint newPos, bool forceGrab = false);
        void closeMagnifier();
        void zoomInMagnifier();
        void zoomOutMagnifier();
        void changeMagnifierMode(int mode);
        void resizedMagnifier(qreal newPercent);

    signals:

       void pageSizeChanged();

    protected:
        UBGraphicsPolygonItem* lineToPolygonItem(const QLineF& pLine, const qreal& pWidth);
        UBGraphicsPolygonItem* arcToPolygonItem(const QLineF& pStartRadius, qreal pSpanAngle, qreal pWidth);

        void initPolygonItem(UBGraphicsPolygonItem*);

        void drawEraser(const QPointF& pEndPoint, bool isFirstDraw = false);
        void drawPointer(const QPointF& pEndPoint, bool isFirstDraw = false);
        void DisposeMagnifierQWidgets();


        virtual void keyReleaseEvent(QKeyEvent * keyEvent);

        void recolorAllItems();

       virtual void drawItems (QPainter * painter, int numItems,
                QGraphicsItem * items[], const QStyleOptionGraphicsItem options[], QWidget * widget = 0);

        QGraphicsItem* rootItem(QGraphicsItem* item) const;

        virtual void drawBackground(QPainter *painter, const QRectF &rect);

    private:
        void setDocumentUpdated();
        void createEraiser();
        void createPointer();
        QString cleanHtml(const QString& _html);

        QGraphicsEllipseItem* mEraser;
        QGraphicsEllipseItem* mPointer;

        QSet<QGraphicsItem*> mAddedItems;
        QSet<QGraphicsItem*> mRemovedItems;

        UBDocumentProxy* mDocument;

        bool mDarkBackground;
        bool mCrossedBackground;
        bool mIsDesktopMode;
        qreal mZoomFactor;

        QGraphicsItem* mBackgroundObject;
        // Issue 1684 - CFA - 20131128
        UBFeatureBackgroundDisposition mBackgroundObjectDisposition;
        QUrl mBackgroundObjectUrl;

        QPointF mPreviousPoint;
        qreal mPreviousWidth;

        QList<UBGraphicsPolygonItem*> mPreviousPolygonItems;

        SceneViewState mViewState;

        bool mInputDeviceIsPressed;

        QSet<QGraphicsItem*> mTools;

        UBGraphicsPolygonItem *mArcPolygonItem;

        QSize mNominalSize;

        RenderingContext mRenderingContext;

        UBGraphicsStroke* mCurrentStroke;

        bool mShouldUseOMP;

        int mItemCount;

        QList<QGraphicsItem*> mFastAccessItems; // a local copy as QGraphicsScene::items() is very slow in Qt 4.6

        //int mMesure1Ms, mMesure2Ms;

        bool mHasCache;
        //        tmp stub for divide addings scene objects from undo mechanism implementation
        bool mUndoRedoStackEnabled;

        UBMagnifier *magniferControlViewWidget;
        UBMagnifier *magniferDisplayViewWidget;

        UBZLayerController *mZLayerController;
        UBGraphicsPolygonItem* mpLastPolygon;

        bool mDrawWithCompass;

};



#endif /* UBGRAPHICSSCENE_H_ */
