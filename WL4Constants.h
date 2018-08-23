#ifndef WL4CONSTANTS_H
#define WL4CONSTANTS_H

namespace WL4Constants
{
    // Definitions for the beginning of tables
    const int TilesetDataTable          = 0x3F2298;
    const int LevelHeaderTable          = 0x639068;
    const int LevelHeaderIndexTable     = 0x6391C4;
    const int LevelNamePointerTable     = 0x63A3AC;
    const int DoorTable                 = 0x78F21C;
    const int RoomDataTable             = 0x78F280;
    const int CameraControlPointerTable = 0x78F540;
    const int EntitySetInfoPointerTable = 0x78EF78;
    const int EntityTilesetPointerTable = 0x78EBF0;
    const int EntityPalettePointerTable = 0x78EDB4;
    const int EntityTilesetLengthTable  = 0x3B2C90;

    // Miscellaneous definitions
    const int CameraRecordSentinel        = 0x3F9D58;
    const int SpritesBasicElementTiles    = 0x400AE8; //0x3000 bytes in length
    const int BGLayerDefaultPtr           = 0x58DA7C;
    const int NormalLayerDefaultPtr       = 0x3F2263;
    const int ToxicLandfillDustyLayer0Ptr = 0x601854;

    // Other
    const int AvailableSpaceBeginningInROM = 0x78F970;

    // BG addresses
    const int BG_0x5FA6D0 = 0x5FA6D0;
    const int BG_0x5FB2CC = 0x5FB2CC;
    const int BG_0x5FB8DC = 0x5FB8DC;
    const int BG_0x5FC2D0 = 0x5FC2D0;
    const int BG_0x5FC9A0 = 0x5FC9A0;
    const int BG_0x5FD078 = 0x5FD078;
    const int BG_0x5FD484 = 0x5FD484;
    const int BG_0x5FD680 = 0x5FD680;
    const int BG_0x5FD9BC = 0x5FD9BC;
    const int BG_0x5FE008 = 0x5FE008;
    const int BG_0x5FE540 = 0x5FE540;
    const int BG_0x5FE918 = 0x5FE918;
    const int BG_0x5FEED8 = 0x5FEED8;
    const int BG_0x5FF264 = 0x5FF264;
    const int BG_0x5FF684 = 0x5FF684;
    const int BG_0x5FF960 = 0x5FF960;
    const int BG_0x5FFD94 = 0x5FFD94;
    const int BG_0x600388 = 0x600388;
    const int BG_0x6006C4 = 0x6006C4;
    const int BG_0x600EF8 = 0x600EF8;
    const int BG_0x6013D4 = 0x6013D4;
    const int BG_0x601A0C = 0x601A0C;
    const int BG_0x60221C = 0x60221C;
    const int BG_0x602858 = 0x602858;
    const int BG_0x603064 = 0x603064;
    const int BG_0x60368C = 0x60368C;
    const int BG_0x603E98 = 0x603E98;
    const int BG_0x6045C4 = 0x6045C4;
    const int BG_0x604ACC = 0x604ACC;
    const int BG_0x605270 = 0x605270;
    const int BG_0x605A7C = 0x605A7C;
    const int BG_0x6063B0 = 0x6063B0;
    const int BG_0x606CF4 = 0x606CF4;
    const int BG_0x6074C4 = 0x6074C4;
    const int BG_0x607CD0 = 0x607CD0;
    const int BG_0x6084DC = 0x6084DC;
    const int BG_0x608CE8 = 0x608CE8;
    const int BG_0x6094F4 = 0x6094F4;
    const int BG_0x609A84 = 0x609A84;
    const int BG_0x60A1D8 = 0x60A1D8;
    const int BG_0x60A1E8 = 0x60A1E8;
    const int BG_0x60A350 = 0x60A350;
    const int BG_0x60A4B4 = 0x60A4B4;
    const int BG_0x60AD10 = 0x60AD10;
    const int BG_0x60B29C = 0x60B29C;
    const int BG_0x60BC54 = 0x60BC54;
    const int BG_0x60C5FC = 0x60C5FC;
    const int BG_0x60CF98 = 0x60CF98;
    const int BG_0x60E044 = 0x60E044;
    const int BG_0x60E860 = 0x60E860;
    const int BG_0x60E870 = 0x60E870;
    const int BG_0x60E96C = 0x60E96C;
    const int BG_0x60ED78 = 0x60ED78;

    //Entity OAM default action frames data pointers
    /*
     * Created by DataExtraction
     * src:DataExtraction.cpp
     * in:Entity data.txt
     * ouput:Entity data(processed).txt
     */
    const int Entity01_0x3B4F94 = 0x3B4F94;
    const int Entity02_0x3B4FA4 = 0x3B4FA4;
    const int Entity03_0x3B4F84 = 0x3B4F84;
    const int Entity04_0x3B4F74 = 0x3B4F74;
    const int Entity05_0x3B4F24 = 0x3B4F24;
    const int Entity06_0x3B4FF4 = 0x3B4FF4;
    const int Entity07_0x3B62AC = 0x3B62AC;
    const int Entity08_0x3B59EC = 0x3B59EC;
    const int Entity09_0x3B492C = 0x3B492C;
    const int Entity10_0x3DA17C = 0x3DA17C;
    const int Entity11_0x3B416C = 0x3B416C;
    const int Entity12_0x3B43DC = 0x3B43DC;
    const int Entity13_0x3B43DC = 0x3B43DC;
    const int Entity14_0x3B4534 = 0x3B4534;
    const int Entity15_0x3C4B18 = 0x3C4B18;
    const int Entity16_0x3B75E8 = 0x3B75E8;
    const int Entity17_0x3B8118 = 0x3B8118;
    const int Entity18_0x3B9AD0 = 0x3B9AD0;
    const int Entity19_0x3BB0E8 = 0x3BB0E8;
    const int Entity1A_0x3BB63C = 0x3BB63C;
    const int Entity1B_0x3BBD6C = 0x3BBD6C;
    const int Entity1B_0x3BBDF4 = 0x3BBDF4;
    const int Entity1C_0x3BC570 = 0x3BC570;
    const int Entity1D_0x3BC570 = 0x3BC570;
    const int Entity1E_0x3BC8E4 = 0x3BC8E4;
    const int Entity1F_0x3BCEFC = 0x3BCEFC;
    const int Entity20_0x3BD42C = 0x3BD42C;
    const int Entity21_0x3BD660 = 0x3BD660;
    const int Entity22_0x3BDAF0 = 0x3BDAF0;
    const int Entity23_0x3BDD54 = 0x3BDD54;
    const int Entity24_0x3BDFF0 = 0x3BDFF0;
    const int Entity25_0x3BDFF0 = 0x3BDFF0;
    const int Entity26_0x3BE1D4 = 0x3BE1D4;
    const int Entity27_0x3BEBF4 = 0x3BEBF4;
    const int Entity28_0x3BFB8C = 0x3BFB8C;
    const int Entity29_0x3B505C = 0x3B505C;
    const int Entity2A_0x3C1270 = 0x3C1270;
    const int Entity2B_0x3C1270 = 0x3C1270;
    const int Entity2C_0x3C302C = 0x3C302C;
    const int Entity2E_0x3C48D4 = 0x3C48D4;
    const int Entity2F_0x3C48F4 = 0x3C48F4;
    const int Entity30_0x3C4174 = 0x3C4174;
    const int Entity31_0x3C41F4 = 0x3C41F4;
    const int Entity32_0x3C4274 = 0x3C4274;
    const int Entity33_0x3C4314 = 0x3C4314;
    const int Entity34_0x3C43B4 = 0x3C43B4;
    const int Entity35_0x3C4444 = 0x3C4444;
    const int Entity36_0x3C44D4 = 0x3C44D4;
    const int Entity37_0x3C4574 = 0x3C4574;
    const int Entity38_0x3C4614 = 0x3C4614;
    const int Entity39_0x3C46B4 = 0x3C46B4;
    const int Entity3A_0x3C47D4 = 0x3C47D4;
    const int Entity3B_0x3C959C = 0x3C959C;
    const int Entity3C_0x3C4984 = 0x3C4984;
    const int Entity3D_0x3C4CD8 = 0x3C4CD8;
    const int Entity3E_0x3C4F20 = 0x3C4F20;
    const int Entity3F_0x3C53B4 = 0x3C53B4;
    const int Entity40_0x3C62FC = 0x3C62FC;
    const int Entity41_0x3C7034 = 0x3C7034;
    const int Entity42_0x3C770C = 0x3C770C;
    const int Entity43_0x3C798C = 0x3C798C;
    const int Entity45_0x3C799C = 0x3C799C;
    const int Entity46_0x3C79AC = 0x3C79AC;
    const int Entity47_0x3C7A5C = 0x3C7A5C;
    const int Entity48_0x3C7DB0 = 0x3C7DB0;
    const int Entity49_0x3C8278 = 0x3C8278;
    const int Entity4A_0x3C89F0 = 0x3C89F0;
    const int Entity4B_0x3C89F0 = 0x3C89F0;
    const int Entity4C_0x3C99E0 = 0x3C99E0;
    const int Entity4D_0x3C9C20 = 0x3C9C20;
    const int Entity4E_0x3CA178 = 0x3CA178;
    const int Entity4F_0x3CA898 = 0x3CA898;
    const int Entity50_0x3CA898 = 0x3CA898;
    const int Entity51_0x3CE468 = 0x3CE468;
    const int Entity52_0x3B505C = 0x3B505C;
    const int Entity53_0x3CFB58 = 0x3CFB58;
    const int Entity54_0x3CFB48 = 0x3CFB48;
    const int Entity55_0x3B4534 = 0x3B4534;
    const int Entity56_0x3B4534 = 0x3B4534;
    const int Entity57_0x3D0B08 = 0x3D0B08;
    const int Entity58_0x3D0938 = 0x3D0938;
    const int Entity59_0x3D0988 = 0x3D0988;
    const int Entity5A_0x3D0A00 = 0x3D0A00;
    const int Entity5B_0x3D0A28 = 0x3D0A28;
    const int Entity5C_0x3D0B58 = 0x3D0B58;
    const int Entity5D_0x3C0A28 = 0x3C0A28;
    const int Entity5E_0x3D0DB4 = 0x3D0DB4;
    const int Entity5F_0x3D0E94 = 0x3D0E94;
    const int Entity60_0x3D155C = 0x3D155C;
    const int Entity61_0x3D21E4 = 0x3D21E4;
    const int Entity62_0x3D2570 = 0x3D2570;
    const int Entity63_0x3D27C8 = 0x3D27C8;
    const int Entity64_0x3D2A0C = 0x3D2A0C;
    const int Entity65_0x3D2BF0 = 0x3D2BF0;
    const int Entity66_0x3D2E1C = 0x3D2E1C;
    const int Entity67_0x3D3048 = 0x3D3048;
    const int Entity68_0x3D3274 = 0x3D3274;
    const int Entity69_0x3DABA0 = 0x3DABA0;
    const int Entity6A_0x3DC264 = 0x3DC264;
    const int Entity6B_0x3DC770 = 0x3DC770;
    const int Entity6C_0x3DCCBC = 0x3DCCBC;
    const int Entity6E_0x3DD720 = 0x3DD720;
    const int Entity6F_0x3DD658 = 0x3DD658;
    const int Entity70_0x3DD668 = 0x3DD668;
    const int Entity71_0x3DD678 = 0x3DD678;
    const int Entity72_0x3DDB14 = 0x3DDB14;
    const int Entity73_0x3DE0E0 = 0x3DE0E0;
    const int Entity74_0x3DE320 = 0x3DE320;
    const int Entity75_0x3DE498 = 0x3DE498;
    const int Entity76_0x3DF2D0 = 0x3DF2D0;
    const int Entity77_0x3E0D68 = 0x3E0D68;
    const int Entity78_0x3E1650 = 0x3E1650;
    const int Entity79_0x3E269C = 0x3E269C;
    const int Entity7A_0x3E2A2C = 0x3E2A2C;
    const int Entity7B_0x3E3878 = 0x3E3878;
    const int Entity7C_0x3E45F0 = 0x3E45F0;
    const int Entity7D_0x3B59EC = 0x3B59EC;
    const int Entity7E_0x3F0F04 = 0x3F0F04;
    const int Entity7F_0x3F122C = 0x3F122C;
    const int Entity80_0x3F1AA0 = 0x3F1AA0;

}

#endif // WL4CONSTANTS_H
