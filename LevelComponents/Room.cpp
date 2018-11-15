#include "Room.h"
#include "WL4Constants.h"
#include "ROMUtils.h"

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <QPainter>
#include <iostream>

#include <WL4EditorWindow.h>
extern WL4EditorWindow *singleton;

namespace LevelComponents
{
    /// <summary>
    /// Construct a new Room object.
    /// </summary>
    /// <param name="roomDataPtr">
    /// Pointer to the start of the room data header.
    /// </param>
    /// <param name="_RoomID">
    /// Zero-based ID for the room in the level.
    /// </param>
    /// <param name="_LevelID">
    /// Level index value from 0x03000023 at run-time.
    /// </param>
    Room::Room(int roomDataPtr, unsigned char _RoomID, unsigned int _LevelID) :
        RoomID(_RoomID),
        LevelID(_LevelID),
        TilesetID(ROMUtils::CurrentFile[roomDataPtr])
    {
        memset(RenderedLayers, 0, sizeof(RenderedLayers));
        memset(drawLayers, 0, sizeof(drawLayers));
        memset(EntityLayerZValue, 0, sizeof(EntityLayerZValue));

        // Copy the room header information
        memcpy(&RoomHeader, ROMUtils::CurrentFile + roomDataPtr, sizeof(struct __RoomHeader));

        // Set up tileset
        int tilesetPtr = WL4Constants::TilesetDataTable + TilesetID * 36;
        tileset = new Tileset(tilesetPtr, TilesetID);

        // Set up the layer data
        int dimensionPointer = ROMUtils::PointerFromData(roomDataPtr + 12);
        Width = ROMUtils::CurrentFile[dimensionPointer];
        Height = ROMUtils::CurrentFile[dimensionPointer + 1];
        for(int i = 0; i < 4; ++i)
        {
            enum LayerMappingType mappingType = static_cast<enum LayerMappingType>(ROMUtils::CurrentFile[roomDataPtr + i + 1] & 0x30);
            int layerPtr = ROMUtils::PointerFromData(roomDataPtr + i * 4 + 8);
            layers[i] = new Layer(layerPtr, mappingType);
        }

        SetLayerPriorityAndAlphaAttributes((int) ROMUtils::CurrentFile[roomDataPtr + 26]);

        // Set up camera control data
        // TODO are there more types than 1, 2 and 3?
        if((CameraControlType = static_cast<enum __CameraControlType>(ROMUtils::CurrentFile[roomDataPtr + 24])) == HasControlAttrs)
        {
            int pLevelCameraControlPointerTable = ROMUtils::PointerFromData(WL4Constants::CameraControlPointerTable + _LevelID * 4);
            for(int i = 0; i < 16; i++)
            {
                int CurrentPointer = ROMUtils::PointerFromData(pLevelCameraControlPointerTable + i * 4);
                if(CurrentPointer == WL4Constants::CameraRecordSentinel)
                    break;
                if(ROMUtils::CurrentFile[CurrentPointer] == _RoomID)
                {
                    int RecordNum = ROMUtils::CurrentFile[CurrentPointer + 1];
                    struct __CameraControlRecord *recordPtr = (struct __CameraControlRecord*) (ROMUtils::CurrentFile + CurrentPointer + 2);
                    while(RecordNum--)
                    {
                        CameraControlRecords.push_back(recordPtr++);
                    }
                    break;
                }
            }
        }

        // Load Entity list for each difficulty level
        for(int i = 0; i < 3; i++)
        {
            int Listaddress = ROMUtils::PointerFromData(roomDataPtr + 28 + 4 * i);
            int k = 0;
            while(ROMUtils::CurrentFile[Listaddress + 3 * k] != (unsigned char) '\xFF') // maximum entity count is 46
            {
                EntityRoomAttribute tmpEntityroomattribute;
                tmpEntityroomattribute.YPos     = (int) ROMUtils::CurrentFile[Listaddress + 3 * k];
                tmpEntityroomattribute.XPos     = (int) ROMUtils::CurrentFile[Listaddress + 3 * k + 1];
                tmpEntityroomattribute.EntityID = (int) ROMUtils::CurrentFile[Listaddress + 3 * k + 2];
                EntityList[i].push_back(tmpEntityroomattribute);
                k++;
            }
        }

        // TODOs: load Entityset and Entities for different difficulties and different Doors.

    }

    /// <summary>
    /// Copy constructor of Room.
    /// </summary>
    /// <remarks>
    /// the new instance should only be used to render Room graphic temporarily, it is unsafe to add it to a Level.
    /// </remarks>
    Room::Room(Room *room)
    {
        RoomID = room->GetRoomID();
        TilesetID = room->GetTilesetID();
        LevelID = room->GetLevelID();
        memset(RenderedLayers, 0, sizeof(RenderedLayers));
        memset(drawLayers, 0, sizeof(drawLayers));
        memset(EntityLayerZValue, 0, sizeof(EntityLayerZValue));

        // Copy the room header information
        RoomHeader = room->GetRoomHeader();

        // Set up tileset
        int tilesetPtr = WL4Constants::TilesetDataTable + TilesetID * 36;
        tileset = new Tileset(tilesetPtr, TilesetID);

        // Set up the layer data
        Width = room->GetWidth();
        Height = room->GetHeight();
        unsigned char *roomheader_charptr = (unsigned char *) &RoomHeader;
        int *roomDataPtr = (int *) (&RoomHeader.Layer0Data);
        for(int i = 0; i < 4; ++i)
        {
            enum LayerMappingType mappingType = static_cast<enum LayerMappingType>(*(roomheader_charptr + i + 1) & 0x30);
            int layerPtr = (*(roomDataPtr + i)) & 0x7FFFFFF;
            layers[i] = new Layer(layerPtr, mappingType);
        }

        SetLayerPriorityAndAlphaAttributes((int) room->GetRoomHeader().LayerEffects);

        // Set up camera control data
        // TODO are there more types than 1, 2 and 3?
        if((CameraControlType = static_cast<enum __CameraControlType>(room->GetRoomHeader().CameraControlType)) == HasControlAttrs)
        {
            int pLevelCameraControlPointerTable = ROMUtils::PointerFromData(WL4Constants::CameraControlPointerTable + LevelID * 4);
            for(int i = 0; i < 16; i++)
            {
                int CurrentPointer = ROMUtils::PointerFromData(pLevelCameraControlPointerTable + i * 4);
                if(CurrentPointer == WL4Constants::CameraRecordSentinel)
                    break;
                if(ROMUtils::CurrentFile[CurrentPointer] == RoomID)
                {
                    int RecordNum = ROMUtils::CurrentFile[CurrentPointer + 1];
                    struct __CameraControlRecord *recordPtr = (struct __CameraControlRecord*) (ROMUtils::CurrentFile + CurrentPointer + 2);
                    while(RecordNum--)
                    {
                        CameraControlRecords.push_back(recordPtr++);
                    }
                    break;
                }
            }
        }

        // Load Entity list for each difficulty level
        for(int i = 0; i < 3; i++)
        {
            int *EntityListaddress = (int *) ((unsigned char *)(&RoomHeader) + 28 + 4 * i);
            int ListAddress = *EntityListaddress - 0x8000000;
            int k = 0;
            while(ROMUtils::CurrentFile[ListAddress + 3 * k] != (unsigned char) '\xFF') // maximum entity count is 46
            {
                EntityRoomAttribute tmpEntityroomattribute;
                tmpEntityroomattribute.YPos     = (int) ROMUtils::CurrentFile[ListAddress + 3 * k];
                tmpEntityroomattribute.XPos     = (int) ROMUtils::CurrentFile[ListAddress + 3 * k + 1];
                tmpEntityroomattribute.EntityID = (int) ROMUtils::CurrentFile[ListAddress + 3 * k + 2];
                EntityList[i].push_back(tmpEntityroomattribute);
                k++;
            }
        }

        // Deep Copy Entityset and Entities
        CurrentEntitySetID = room->GetCurrentEntitySetID();
        ResetEntitySet(CurrentEntitySetID);
    }

    /// <summary>
    /// Deconstruct drawLayers[].
    /// </summary>
    void Room::FreeDrawLayers()
    {
        for(unsigned int i = 0; i < sizeof(drawLayers) / sizeof(drawLayers[0]); ++i)
        {
            if(drawLayers[i])
            {
                delete drawLayers[i];
            }
        }
    }

    /// <summary>
    /// release Entity instances list in this Room.
    /// </summary>
    void Room::FreecurrentEntityListSource()
    {
        int i = (int) currentEntityListSource.size();
        if(i < 1) return;
        for(int j = 0; j < i; ++j)
        {
            Entity *currententity = currentEntityListSource[j];
            delete currententity;
        }
        currentEntityListSource.clear();
    }

    /// <summary>
    /// Reset the EntitySet in this Room.
    /// </summary>
    /// <param name="entitysetId">
    /// New EntitySet Id.
    /// </param>
    void Room::ResetEntitySet(int entitysetId)
    {
        if(currentEntitySet != nullptr) delete currentEntitySet;
        FreecurrentEntityListSource();
        currentEntitySet = new EntitySet(entitysetId, tileset->GetUniversalSpritesTilesPalettePtr());
//        currentEntitySet = new EntitySet(entitysetId, WL4Constants::UniversalSpritesPalette);
        for(int i = 0; i < 17; ++i)
        {
            Entity *newEntity = new Entity(-1, i, currentEntitySet);
            currentEntityListSource.push_back(newEntity);
        }
        for(int i = 0; i < (int) currentEntitySet->GetEntityTable().size(); ++i)
        {
            int _globalId = currentEntitySet->GetEntityTable().at(i).Global_EntityID;
            Entity *newEntity = new Entity(i, _globalId, currentEntitySet);
            currentEntityListSource.push_back(newEntity);
        }
    }

    /// <summary>
    /// Deconstruct an instance of Room.
    /// </summary>
    Room::~Room()
    {
        // Free drawlayer elements
        FreeDrawLayers();
        if(currentEntitySet != nullptr) delete currentEntitySet;
        FreecurrentEntityListSource();
        delete tileset;
    }

    /// <summary>
    /// Get distributed doors' pointers from Room class and Reset EntitySet in this Room by the way if needed.
    /// </summary>
    /// <param name="newdoor">
    /// eExisting Door ptr.
    /// </param>
    void Room::PushBack_Door(Door *newdoor)
    {
         doors.push_back(newdoor);
         if(CurrentEntitySetID == 0)
         {
             CurrentEntitySetID = newdoor->GetEntitySetID();
             ResetEntitySet(CurrentEntitySetID);
         }
    }

    /// <summary>
    /// Render an entire graphics scene for the Room.
    /// </summary>
    /// <remarks>
    /// There are different ways to render the graphics for the room; these ways are defined in
    /// the LevelComponents::RenderUpdateType enum, and the parameters for it are stored in <paramref name="renderParams"/>.
    /// </remarks>
    /// <param name="scene">
    /// The graphics scene object which will be fully rendered, or contain pre-rendered graphics to re-render.
    /// </param>
    /// <param name="renderParams">
    /// A struct containing the parameters for how the scene should be rendered or re-rendered.
    /// </param>
    /// <return>
    /// A graphics scene containing fully rendered pixmap layers in proper Z order.
    /// </return>
    QGraphicsScene *Room::RenderGraphicsScene(QGraphicsScene *scene, struct RenderUpdateParams *renderParams)
    {
        int sceneWidth = Width * 16;
        int sceneHeight = Height * 16;
        int Z = 0;
        switch(renderParams->type)
        {
        case FullRender:
            {
                // Order the layers by their priority
                FreeDrawLayers();
                for(int i = 0; i < 4; ++i)
                {
                    struct DLS *newDLS = new struct DLS;
                    newDLS->layer = layers[i];
                    newDLS->index = i;
                    drawLayers[i] = newDLS;
                }
                qsort(drawLayers, 4, sizeof(void*), [](const void *data1, const void *data2)
                {
                    struct DLS *layer1 = *(struct DLS**) data1;
                    struct DLS *layer2 = *(struct DLS**) data2;
                    return layer2->layer->GetLayerPriority() - layer1->layer->GetLayerPriority();
                });

                // Create a graphics scene with the layers added in order of priority
                if(scene) { delete scene; } // Make a new graphics scene to draw to
                scene = new QGraphicsScene(0, 0, qMax(sceneWidth, 16 * this->GetLayer(0)->GetLayerWidth()), sceneHeight);

                // This represents the EVA alpha layer, which will be rendered in passes before the alpha layer is finalized
                QPixmap alphaPixmap(sceneWidth, sceneHeight);
                QPainter alphaPainter(&alphaPixmap);

                // Render the 4 layers in the order of their priority
                bool *LayersCurrentVisibility = singleton->GetLayersVisibilityArray();
                for(int i = 0; i < 4; ++i)
                {
                    QPixmap pixmap = drawLayers[i]->layer->RenderLayer(tileset);
                    // If this is a layer composed of 8x8 tiles, then repeat the layer in X and Y to the size of the other layers
                    if(drawLayers[i]->layer->GetMappingType() == LayerTile8x8)
                    {
                        QPixmap pixmap2(sceneWidth, sceneHeight);
                        QPainter painter(&pixmap2);
                        for(int y = 0; y < sceneHeight; y += drawLayers[i]->layer->GetLayerHeight() * 8)
                        {
                            for(int x = 0; x < sceneWidth; x += drawLayers[i]->layer->GetLayerWidth() * 8)
                            {
                                QPoint drawDestination(x, y);
                                painter.drawImage(drawDestination, pixmap.toImage());
                            }
                        }
                        pixmap = pixmap2;
                    }

                    // Add the rendered layer to the graphics scene
                    QGraphicsPixmapItem *pixmapItem = scene->addPixmap(pixmap);
                    pixmapItem->setZValue(Z); Z += 2;
                    EntityLayerZValue[3 - i] = Z - 1;
                    RenderedLayers[drawLayers[i]->index] = pixmapItem;

                    // Render alpha blended composite pixmap for layer 0 if alpha blending is enabled
                    if(Layer0ColorBlending && (Layer0ColorBlendCoefficient_EVB != 0))
                    {
                        // If this is a pass for a layer under the alpha layer, draw the rendered layer to the EVA component image
                        if((3 - i) > layers[0]->GetLayerPriority() && LayersCurrentVisibility[drawLayers[i]->index])
                            alphaPainter.drawImage(0, 0, RenderedLayers[drawLayers[i]->index]->pixmap().toImage());
                        else if((3 - i) == layers[0]->GetLayerPriority())
                        {
                            // Blend the EVA and EVB pixels for the new layer
                            Z--;
                            QImage imageA = RenderedLayers[0]->pixmap().toImage();
                            QImage imageB = alphaPixmap.toImage();
                            for(int j = 0; j < sceneHeight; ++j)
                            {
                                for(int k = 0; k < sceneWidth; ++k)
                                {
                                    QColor PXA = QColor(imageA.pixel(k, j)), PXB = QColor(imageB.pixel(k, j));
                                    int R = qMin((Layer0ColorBlendCoefficient_EVA * PXA.red()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.red()) / 16, 255);
                                    int G = qMin((Layer0ColorBlendCoefficient_EVA * PXA.green()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.green()) / 16, 255);
                                    int B = qMin((Layer0ColorBlendCoefficient_EVA * PXA.blue()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.blue()) / 16, 255);
                                    imageA.setPixel(k, j, QColor(R, G, B).rgb());
                                }
                            }

                            // Add the alpha pixmap above the non-blended layer 0, but below the next one to be rendered
                            QGraphicsPixmapItem *alphaItem = scene->addPixmap(QPixmap::fromImage(imageA));
                            alphaItem->setZValue(Z); Z += 2;
                            EntityLayerZValue[i] = Z - 1;
                            RenderedLayers[7] = alphaItem;
                        };
                    }
                    else RenderedLayers[7] = nullptr;
                }
                delete[] LayersCurrentVisibility;
            }
            // Fall through to ElementsLayersUpdate section
        case ElementsLayersUpdate:
            {
                // Render entity layer
                QPixmap *EntityPixmap[4];
                QPainter *EntityPainter[4];
                for(int i = 0; i < 4; ++i)
                {
                    EntityPixmap[i] = new QPixmap(sceneWidth, sceneHeight);
                    EntityPixmap[i]->fill(Qt::transparent);
                    EntityPainter[i] = new QPainter(EntityPixmap[i]);
                }
                currentDifficulty = renderParams->mode.seleteddifficulty;
                for(int i = 0; i < (int) EntityList[currentDifficulty].size(); ++i)
                {
                    Entity *currententity = currentEntityListSource[EntityList[currentDifficulty].at(i).EntityID];
//                    EntityPainter[3 - currententity->GetPriority()]->drawImage(
                    // Use an alternative method to render the Entity in a not-so-bad place
                    if((Layer0ColorBlendCoefficient_EVB == 0) && (Layer0ColorBlending == true))
                    {
                        int tmppriority = (layers[1]->GetLayerPriority()) > (layers[2]->GetLayerPriority()) ? layers[1]->GetLayerPriority(): (layers[2]->GetLayerPriority());
                        EntityPainter[tmppriority]->drawImage(
                                    16 * EntityList[currentDifficulty][i].XPos + currententity->GetXOffset() + 8 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).XOffset + 98) / 4,
                                    16 * EntityList[currentDifficulty][i].YPos + currententity->GetYOffset() + 16 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).YOffset + 66) / 4,
                                    currententity->Render());
                    }
                    else if(/*(layers[0]->GetLayerPriority() != 0) && */(Layer0ColorBlending == true) && (Layer0ColorBlendCoefficient_EVB != 0))
                    {
                        EntityPainter[layers[0]->GetLayerPriority()]->drawImage(
                                    16 * EntityList[currentDifficulty][i].XPos + currententity->GetXOffset() + 8 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).XOffset + 98) / 4,
                                    16 * EntityList[currentDifficulty][i].YPos + currententity->GetYOffset() + 16 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).YOffset + 66) / 4,
                                    currententity->Render());
                    }
                    else
                    {
                        EntityPainter[layers[1]->GetLayerPriority() + 1]->drawImage(
                                    16 * EntityList[currentDifficulty][i].XPos + currententity->GetXOffset() + 8 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).XOffset + 98) / 4,
                                    16 * EntityList[currentDifficulty][i].YPos + currententity->GetYOffset() + 16 + (currentEntitySet->GetEntityPositionalOffset(currententity->GetEntityGlobalID()).YOffset + 66) / 4,
                                    currententity->Render());
                    }
                }
                for(int i = 0; i < 4; ++i)
                {
                    QGraphicsPixmapItem *EntityItem;
                    if(!RenderedLayers[8 + i] || renderParams->type == FullRender)
                    {
                        EntityItem = scene->addPixmap(*EntityPixmap[i]);
                        EntityItem->setZValue(EntityLayerZValue[i]);
                        RenderedLayers[8 + i] = EntityItem;
                    }
                    else
                    {
                        RenderedLayers[8 + i]->setPixmap(*EntityPixmap[i]);
                    }
                    delete EntityPainter[i];
                    delete EntityPixmap[i];
                }

                // Reset Z value
                Z = (Layer0ColorBlending && Layer0ColorBlendCoefficient_EVB) ? 9 : 8;

                // Render door layer
                QPixmap doorPixmap(sceneWidth, sceneHeight);
                doorPixmap.fill(Qt::transparent);
                QPainter doorPainter(&doorPixmap);
                QPen DoorPen = QPen(QBrush(Qt::blue), 2);
                DoorPen.setJoinStyle(Qt::MiterJoin);
                doorPainter.setPen(DoorPen);
                for(unsigned int i = 0; i < doors.size(); i++)
                {
                    int doorX = doors[i]->GetX1() * 16;
                    int doorY = doors[i]->GetY1() * 16;
                    int doorWidth = (qAbs(doors[i]->GetX1() - doors[i]->GetX2()) + 1) * 16;
                    int doorHeight = (qAbs(doors[i]->GetY1() - doors[i]->GetY2()) + 1) * 16;
                    if(i == renderParams->SelectedDoorID)
                    {
                        QPen DoorPen2 = QPen(QBrush(Qt::cyan), 2);
                        DoorPen2.setJoinStyle(Qt::MiterJoin);
                        doorPainter.setPen(DoorPen2);
                        doorPainter.drawRect(doorX, doorY, doorWidth, doorHeight);
                        doorPainter.fillRect(doorX + 1, doorY + 1, doorWidth - 2, doorHeight - 2, QColor(0, 0xFF, 0xFF, 0x5F));
                        doorPainter.setPen(DoorPen);
                    }
                    else
                    {
                        doorPainter.drawRect(doorX, doorY, doorWidth, doorHeight);
                        doorPainter.fillRect(doorX + 1, doorY + 1, doorWidth - 2, doorHeight - 2, QColor(0, 0, 0xFF, 0x5F));
                    }
                }
                QGraphicsPixmapItem *doorpixmapItem;
                if(!RenderedLayers[5] || renderParams->type == FullRender)
                {
                    doorpixmapItem = scene->addPixmap(doorPixmap);
                    doorpixmapItem->setZValue(Z++);
                    RenderedLayers[5] = doorpixmapItem;
                }
                else
                {
                    RenderedLayers[5]->setPixmap(doorPixmap);
                }

                // Render camera box layer
                QPixmap CameraLimitationPixmap(sceneWidth, sceneHeight);
                CameraLimitationPixmap.fill(Qt::transparent);
                QPainter CameraLimitationPainter(&CameraLimitationPixmap);
                CameraLimitationPainter.setRenderHint(QPainter::Antialiasing);
                QPen CameraLimitationPen = QPen(QBrush(Qt::red), 2);
                QPen CameraLimitationPen2 = QPen(QBrush(Qt::green), 2);
                CameraLimitationPen.setJoinStyle(Qt::MiterJoin);
                CameraLimitationPen2.setJoinStyle(Qt::MiterJoin);
                CameraLimitationPainter.setPen(CameraLimitationPen);

                if(CameraControlType == LevelComponents::FixedY)
                {
                    // Use Wario original position when getting out of a door to figure out the Camera Limitator Y position
                    // CameraY and WarioYPos here are 4 times the real values
                    int CameraY = 0x80 - 32;
                    int WarioYPos = doors[0]->GetWarioOriginalPosition_x4().y(); // Use the first door in the data
                    if(WarioYPos > 0x260)
                    {
                        do
                        {
                            CameraY += 0x240;
                        } while(WarioYPos > (CameraY + 0x280));
                    }

                    // Force the value to be normal
                    CameraY = CameraY / 4;

                    // Get the first Camera limitator Y value
                    while (CameraY > 0xA0)
                    {
                        CameraY -= 0x90;
                    }

                    // Draw Camera Limitation
                    while ((CameraY + 0xA0) < (int) (Height * 16))
                    {
                        CameraLimitationPainter.drawRect(0x20, CameraY, (int) Width * 16 - 0x40, 0xA0);
                        CameraY += 0x90;
                    }
                }
                else if(CameraControlType == LevelComponents::NoLimit)
                {
                    CameraLimitationPainter.drawRect(0x20, 0x20, (int) Width * 16 - 0x40, (int) Height * 16 - 0x40);
                }
                else if(CameraControlType == LevelComponents::HasControlAttrs)
                {
                    for(unsigned int i = 0; i < CameraControlRecords.size(); i++)
                    {
                        CameraLimitationPainter.drawRect(
                            16 * ((int) CameraControlRecords[i]->x1) + 1,
                            16 * ((int) CameraControlRecords[i]->y1) + 1,
                            16 * (qMin((int) CameraControlRecords[i]->x2, (int) Width - 3) - (int) CameraControlRecords[i]->x1 + 1) - 2,
                            16 * (qMin((int) CameraControlRecords[i]->y2, (int) Height - 3) - (int) CameraControlRecords[i]->y1 + 1) - 2
                        );
                        if(CameraControlRecords[i]->x3 != (unsigned char) '\xFF')
                        {
                            // Draw a box around the block which triggers the camera box, and a line connecting it
                            CameraLimitationPainter.drawRect(
                                16 * ((int) CameraControlRecords[i]->x3) + 2,
                                16 * ((int) CameraControlRecords[i]->y3) + 2,
                                12,
                                12
                            );
                            CameraLimitationPainter.drawLine(
                                16 * ((int) CameraControlRecords[i]->x1) + 1,
                                16 * ((int) CameraControlRecords[i]->y1) + 1,
                                16 * ((int) CameraControlRecords[i]->x3) + 2,
                                16 * ((int) CameraControlRecords[i]->y3) + 2
                            );
                            CameraLimitationPainter.setPen(CameraLimitationPen2);
                            int SetNum[4] =
                            {
                                (int) CameraControlRecords[i]->x1,
                                (int) CameraControlRecords[i]->x2,
                                (int) CameraControlRecords[i]->y1,
                                (int) CameraControlRecords[i]->y2
                            };
                            int k = (int) CameraControlRecords[i]->ChangeValueOffset;
                            SetNum[k] = (int) CameraControlRecords[i]->ChangedValue;
                            CameraLimitationPainter.drawRect(
                                16 * SetNum[0],
                                16 * SetNum[2],
                                16 * (qMin(SetNum[1], (int) Width - 3) - SetNum[0] + 1),
                                16 * (qMin(SetNum[3], (int) Height - 3) - SetNum[2] + 1)
                            );
                            CameraLimitationPainter.setPen(CameraLimitationPen);
                        }
                    }
                }
                else
                {
                    // TODO other camera control type
                }
                QGraphicsPixmapItem *CameraLimitationpixmapItem;
                if(!RenderedLayers[6] || renderParams->type == FullRender)
                {
                    CameraLimitationpixmapItem = scene->addPixmap(CameraLimitationPixmap);
                    CameraLimitationpixmapItem->setZValue(Z++);
                    RenderedLayers[6] = CameraLimitationpixmapItem;
                }
                else
                {
                    RenderedLayers[6]->setPixmap(CameraLimitationPixmap);
                }

                // TODO: Render Entities Boxes used for selecting
                QPixmap EntityBoxPixmap(sceneWidth, sceneHeight);
                EntityBoxPixmap.fill(Qt::transparent);
                QPainter EntityBoxPainter(&EntityBoxPixmap);
                QPen EntityBoxPen = QPen(QBrush(QColor(0xFF, 0xFF, 0, 0xFF)), 2);
                EntityBoxPen.setJoinStyle(Qt::MiterJoin);
                EntityBoxPainter.setPen(EntityBoxPen);
                for(int i = 0; i < (int) EntityList[currentDifficulty].size(); ++i)
                {
                    if(i == renderParams->SelectedEntityID)
                    {
                        QPen EntityBoxPen2 = QPen(QBrush(QColor(0xFF, 0x7F, 0, 0xFF)), 2);
                        EntityBoxPen2.setJoinStyle(Qt::MiterJoin);
                        EntityBoxPainter.setPen(EntityBoxPen2);
                        EntityBoxPainter.drawRect(16 * EntityList[currentDifficulty][i].XPos, 16 * EntityList[currentDifficulty][i].YPos, 16, 16);
                        EntityBoxPainter.setPen(EntityBoxPen);
                    }
                    else
                    {
                        EntityBoxPainter.drawRect(16 * EntityList[currentDifficulty][i].XPos, 16 * EntityList[currentDifficulty][i].YPos, 16, 16);
                    }

                }
                // Test: Render EntitySet Tiles in the front of the Layer RenderedLayers[4]
                //EntityBoxPainter.drawPixmap(0, 0, currentEntitySet->GetPixmap(9));
                QGraphicsPixmapItem *EntityBoxpixmapItem;
                if(!RenderedLayers[4] || renderParams->type == FullRender)
                {
                    EntityBoxpixmapItem = scene->addPixmap(EntityBoxPixmap);
                    EntityBoxpixmapItem->setZValue(Z++);
                    RenderedLayers[4] = EntityBoxpixmapItem;
                }
                else
                {
                    RenderedLayers[4]->setPixmap(EntityBoxPixmap);
                }
            }
            // Fall through to layer enable section
        case LayerEnable:
            {
                // Enable visibility of the foreground and background layers
                Ui::EditModeParams *layerVisibility = &(renderParams->mode);
                for(int i = 0; i < 4; ++i)
                {
                    RenderedLayers[i]->setVisible(layerVisibility->layersEnabled[i]);
                }

                // Enable the visibility of the sprite and editor overlay layers
                if(RenderedLayers[4])
                {
                    RenderedLayers[4]->setVisible(layerVisibility->entitiesEnabled);
                    RenderedLayers[8]->setVisible(layerVisibility->entitiesEnabled);
                    RenderedLayers[9]->setVisible(layerVisibility->entitiesEnabled);
                    RenderedLayers[10]->setVisible(layerVisibility->entitiesEnabled);
                    RenderedLayers[11]->setVisible(layerVisibility->entitiesEnabled);
                }
                if(RenderedLayers[5]) RenderedLayers[5]->setVisible(layerVisibility->doorsEnabled);
                if(RenderedLayers[6]) RenderedLayers[6]->setVisible(layerVisibility->cameraAreasEnabled);
                if(RenderedLayers[7]) RenderedLayers[7]->setVisible(layerVisibility->alphaBlendingEnabled);
            }
            return scene;
        case SingleTile:
            {
                // Re-render the QImage for the changed tile
                Layer *layer = layers[renderParams->mode.selectedLayer];
                if(layer->IsEnabled() == false) return scene;
                layer->ReRenderTile(renderParams->tileX, renderParams->tileY, renderParams->tileID, tileset);

                // Obtain the old QPixmap from the previously-rendered graphic layers
                QGraphicsPixmapItem *item = RenderedLayers[renderParams->mode.selectedLayer];

                // Draw the new tile graphics over the position of the old tile in the QPixmap
                QPixmap pm(item->pixmap());
                int units = layer->GetMappingType() == LayerMap16 ? 16 : 8;
                int X = renderParams->tileX * units;
                int Y = renderParams->tileY * units;
                int tileDataIndex = renderParams->tileX + renderParams->tileY * layer->GetLayerWidth();
                layer->GetTiles()[tileDataIndex]->DrawTile(&pm, X, Y);

                // Set the new QPixmap for the graphics item on the QGraphicsScene
                item->setPixmap(pm);

                // Redraw alpha layer
                if(Layer0ColorBlending && (Layer0ColorBlendCoefficient_EVB != 0))
                {
                    QGraphicsPixmapItem *alphalayeritem = RenderedLayers[7];
                    Layer *layerqueue[4];
                    QList<Layer*> layerlist;
                    layerlist.push_back(layers[0]); layerlist.push_back(layers[1]); layerlist.push_back(layers[2]); layerlist.push_back(layers[3]);

                    qSort(layerlist.begin(), layerlist.end(), [](Layer *layera, Layer *layerb)
                    {
                        return layera->GetLayerPriority() < layerb->GetLayerPriority();
                    });
                    for(int i = 0; i < 4; i++)
                    {
                        layerqueue[i] = layerlist.at(3 - i);
                    }

                    QPixmap alphaPixmapTemp = alphalayeritem->pixmap();
                    QPainter alphaPainterTemp(&alphaPixmapTemp);
                    bool *LayersCurrentVisibilityTemp = singleton->GetLayersVisibilityArray();

                    for(int i = 0; i < 4; i++)
                    {
                        // If this is a pass for a layer under the alpha layer, draw the rendered layer to the EVA component image
                        if((layerqueue[i] != layers[0]) && LayersCurrentVisibilityTemp[drawLayers[i]->index])
                        {
                            QPixmap pm_tmp = RenderedLayers[drawLayers[i]->index]->pixmap().copy(renderParams->tileX * 16, renderParams->tileY * 16, 16, 16);
                            alphaPainterTemp.drawImage(renderParams->tileX * 16,
                                                       renderParams->tileY * 16,
                                                       pm_tmp.toImage());
                        }
                        else if(layerqueue[i] == layers[0])
                        {
                            // Blend the EVA and EVB pixels for the new layer
                            QImage imageA = RenderedLayers[0]->pixmap().toImage();
                            QImage imageB = alphaPixmapTemp.toImage();
                            for(int j = 16 * renderParams->tileY; j < (16 * renderParams->tileY + 16); ++j)
                            {
                                for(int k = 16 * renderParams->tileX; k < (16 * renderParams->tileX + 16); ++k)
                                {
                                    QColor PXA = QColor(imageA.pixel(k, j)), PXB = QColor(imageB.pixel(k, j));
                                    int R = qMin((Layer0ColorBlendCoefficient_EVA * PXA.red()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.red()) / 16, 255);
                                    int G = qMin((Layer0ColorBlendCoefficient_EVA * PXA.green()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.green()) / 16, 255);
                                    int B = qMin((Layer0ColorBlendCoefficient_EVA * PXA.blue()) / 16 + (Layer0ColorBlendCoefficient_EVB * PXB.blue()) / 16, 255);
                                    imageB.setPixel(k, j, QColor(R, G, B).rgb());
                                }
                            }
                            alphalayeritem->setPixmap(QPixmap::fromImage(imageB));
                            break;
                        };

                    }
                    delete[] LayersCurrentVisibilityTemp;
                }
            }
            return scene;
        }
        // ERROR
        return nullptr;
    }

    /// <summary>
    /// Set Layers Priority and Alpha Attributes for Layer instances and class members in Room (this) class.
    /// </summary>
    void Room::SetLayerPriorityAndAlphaAttributes(int layerPriorityAndAlphaAttr)
    {
        // Prioritize the layers
        int priorityFlag = layerPriorityAndAlphaAttr;
        RoomHeader.LayerEffects = (unsigned char) layerPriorityAndAlphaAttr;
        switch(priorityFlag & 3)
        {
        case 0:
            layers[0]->SetLayerPriority(0);
            layers[1]->SetLayerPriority(1);
            layers[2]->SetLayerPriority(2);
            break;
        case 1:
        case 2:
            layers[0]->SetLayerPriority(1);
            layers[1]->SetLayerPriority(0);
            layers[2]->SetLayerPriority(2);
            break;
        case 3:
            layers[0]->SetLayerPriority(2);
            layers[1]->SetLayerPriority(0);
            layers[2]->SetLayerPriority(1);
        }
        layers[3]->SetLayerPriority(3);

        // Get the information about Layer 0 color blending, using priorityFlag
        if((Layer0ColorBlending = priorityFlag > 7))
        {
            switch((priorityFlag - 8) >> 2)
            {
            case  0: Layer0ColorBlendCoefficient_EVA =  7; Layer0ColorBlendCoefficient_EVB = 16; break;
            case  1: Layer0ColorBlendCoefficient_EVA = 10; Layer0ColorBlendCoefficient_EVB = 16; break;
            case  2: Layer0ColorBlendCoefficient_EVA = 13; Layer0ColorBlendCoefficient_EVB = 16; break;
            case  3: Layer0ColorBlendCoefficient_EVA = 16; Layer0ColorBlendCoefficient_EVB = 16; break;
            case  4: Layer0ColorBlendCoefficient_EVA = 16; Layer0ColorBlendCoefficient_EVB =  0; break;
            case  5: Layer0ColorBlendCoefficient_EVA = 13; Layer0ColorBlendCoefficient_EVB =  3; break;
            case  6: Layer0ColorBlendCoefficient_EVA = 10; Layer0ColorBlendCoefficient_EVB =  6; break;
            case  7: Layer0ColorBlendCoefficient_EVA =  7; Layer0ColorBlendCoefficient_EVB =  9; break;
            case  8: Layer0ColorBlendCoefficient_EVA =  5; Layer0ColorBlendCoefficient_EVB = 11; break;
            case  9: Layer0ColorBlendCoefficient_EVA =  3; Layer0ColorBlendCoefficient_EVB = 13; break;
            case 10: Layer0ColorBlendCoefficient_EVA =  0; Layer0ColorBlendCoefficient_EVB = 16;
            }
        }
    }

    /// <summary>
    /// Get the layer data pointer for a layer.
    /// </summary>
    /// <remarks>
    /// The pointer starts in the 0x8000000 range, so the 28th bit is set to 0 to normalize the address.
    /// </remarks>
    /// <param name="LayerNum">
    /// The number of the layer to retrieve the data pointer for.
    /// </param>
    /// <return>
    /// The normalized data pointer for the requested layer.
    /// </return>
    int Room::GetLayerDataPtr(int LayerNum)
    {
        assert(!(LayerNum & 0xFFFFFFFC)); // 0 <= LayerNum < 4
        return ((unsigned int*)(&RoomHeader.Layer0Data))[LayerNum] & 0x7FFFFFF;
    }

    /// <summary>
    /// Set Layer data pointer into the Room header struct in sake of saving changes.
    /// </summary>
    void Room::SetLayerDataPtr(int LayerNum, int dataPtr)
    {
        assert(!(LayerNum & 0xFFFFFFFC));
        ((unsigned int*)(&RoomHeader.Layer0Data))[LayerNum] = dataPtr;
    }

    /// <summary>
    /// Set attributes relate to layer 3 autoscroll in Room header struct in sake of saving changes.
    /// </summary>
    void Room::SetBGLayerAutoScrollEnabled(bool enability)
    {
        if(enability)
        {
            RoomHeader.Layer3Scrolling = (unsigned char) 7;
        }
        else
        {
            if(RoomHeader.Layer3MappingType == (unsigned char) 0)
            {
                RoomHeader.Layer3Scrolling = (unsigned char) 3;
            }
            else
            {
                RoomHeader.Layer3Scrolling = (unsigned char) 1;
            }
        }
    }

    int Room::GetLocalDoorID(int globalDoorId)
    {
        for(unsigned int i = 0; i < doors.size(); ++i)
        {
            if(doors[i]->GetGlobalDoorID() == globalDoorId)
            return (int) i;
        }
        return -1; // TODO: Error handling
    }

    void Room::Save(QVector<struct ROMUtils::SaveData> chunks, struct ROMUtils::SaveData *headerChunk)
    {
        // Calculate some values needed to initialize the save data
        unsigned int doorTablePtr = WL4Constants::DoorTable + LevelID * 4;
        unsigned int cameraChunkSize = CameraControlRecords.size() * sizeof(struct __CameraControlRecord);

        // Populate layer chunks (uses chunk-relative addresses)
        unsigned int *layerPtrs = (unsigned int*)(headerChunk->data + RoomID * sizeof(struct __RoomHeader) + 8);
        for(unsigned int i = 0; i < 4; ++i)
        {
            Layer *layer = layers[i];
            if(layer->IsDirty())
            {
                // Add the data for this layer, it must be compressed
                unsigned int compressedSize;
                unsigned char *compressedData = layer->GetCompressedLayerData(&compressedSize);
                struct ROMUtils::SaveData layerChunk =
                {
                    RoomID * sizeof(struct __RoomHeader) + 8 + i * 4,
                    compressedSize,
                    compressedData,
                    ROMUtils::SaveDataIndex++,
                    true,
                    headerChunk->index
                };
                chunks.append(layerChunk);
            }
            else
            {
                // Write the old layer data pointer to the header
                layerPtrs[i] = GetLayerDataPtr(i);
            }
        }

        // Create camera boundary chunk
        struct ROMUtils::SaveData cameraChunk =
        {
            doorTablePtr,
            cameraChunkSize,
            (unsigned char*) malloc(cameraChunkSize),
            ROMUtils::SaveDataIndex++,
            false,
            0
        };

        // Populate camera boundary chunk with data
        for(unsigned int i = 0; i < CameraControlRecords.size(); ++i)
        {
            struct __CameraControlRecord *ccr = CameraControlRecords[i];
            // TODO
        }

        chunks.append(cameraChunk);
    }

    __RoomHeader::__RoomHeader(Room *room) :
        TilesetID(room->GetTilesetID()),
        Layer0MappingType(room->GetLayer(0)->GetMappingType()),
        Layer1MappingType(room->GetLayer(1)->GetMappingType()),
        Layer2MappingType(room->GetLayer(2)->GetMappingType()),
        Layer3MappingType(room->GetLayer(3)->GetMappingType()),
        Layer0Data(0), // set manually
        Layer1Data(0),
        Layer2Data(0),
        Layer3Data(0),
        CameraControlType(room->GetCameraControlType()),
        Layer3Scrolling(room->GetBGScrollParameter()),
        LayerEffects(room->GetLayerEffectsParam()),
        DATA_1B(0),
        EntityTableHard(0), // set manually
        EntityTableNormal(0),
        EntityTableSHard(0) {}

    /// <summary>
    /// Check if there is an Entity exist in one place.
    /// </summary>
    /// <param name="XPos">
    /// The X position of the place.
    /// </param>
    /// <param name="YPos">
    /// The Y position of the place.
    /// </param>
    /// <returns>
    /// The index of the entity at the location, or -1 if there is none.
    /// </returns>
    int Room::FindEntity(int XPos, int YPos)
    {
        int Entitynum = EntityList[currentDifficulty].size();
        for(int i = 0; i < Entitynum; ++i)
        {
            if((EntityList[currentDifficulty][i].XPos == XPos) && (EntityList[currentDifficulty][i].YPos == YPos)) return i;
        }
        return -1;
    }

    /// <summary>
    /// Add a new Entity into the current Entity List.
    /// </summary>
    /// <param name="XPos">
    /// The X position of the place.
    /// </param>
    /// <param name="YPos">
    /// The Y position of the place.
    /// </param>
    /// <param name="localEntityId">
    /// The local Id of the Entity in the current EntitySet.
    /// </param>
    /// <returns>
    /// Always true (?)
    /// </returns>
    bool Room::AddEntity(int XPos, int YPos, int localEntityId)
    {
        if(EntityList[currentDifficulty].size() == (int) 47) return false;
        EntityRoomAttribute newEntityattrs;
        newEntityattrs.XPos = XPos;
        newEntityattrs.YPos = YPos;
        newEntityattrs.EntityID = localEntityId;
        EntityList[currentDifficulty].push_back(newEntityattrs);
        return true;
    }

    /// <summary>
    /// Delete an Entity from a Entity List.
    /// </summary>
    /// <param name="index">
    /// The index of the Entity record in EntityList[currentDifficulty], count from 0.
    /// </param>
    void Room::DeleteEntity(int index)
    {
        EntityList[currentDifficulty].erase(EntityList[currentDifficulty].begin() + index);
    }

    /// <summary>
    /// Delete an Door from the Room.
    /// </summary>
    /// <param name="globalDoorIndex">
    /// The global Door id given by current Level.
    /// </param>
    void Room::DeleteDoor(int globalDoorIndex)
    {
        for(unsigned int i = 0; i < doors.size(); ++i)
        {
            if(doors[i]->GetGlobalDoorID() == globalDoorIndex)
            {
                doors.erase(doors.begin() + i);
                return;
            }
        }
    }
}
