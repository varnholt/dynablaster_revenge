#include <QtCore/QCoreApplication>

#include "astarmap.h"
#include "astarpathfinding.h"
#include "botmap.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString testMap1 =
      "#t###########\n"  // 0
      "# ###########\n"  // 1
      "# ###########\n"  // 2
      "# ###########\n"  // 3
      "# ###########\n"  // 4
      "# ###########\n"  // 5
      "# ###########\n"  // 6
      "#          ##\n"  // 7
      "########## ##\n"  // 8
      "########## ##\n"  // 9
      "##########s##\n"; // 10

    QString testMap3 =
      "#############\n"  // 0
      "#     t    ##\n"  // 1
      "# ######## ##\n"  // 2
      "# ######## ##\n"  // 3
      "# ######## ##\n"  // 4
      "# ######## ##\n"  // 5
      "# ######## ##\n"  // 6
      "#          ##\n"  // 7
      "########## ##\n"  // 8
      "########## ##\n"  // 9
      "##########s##\n"; // 10

    QString testMap4 =
    // 0123456789abc
      "#############\n"  // 0
      "# t         #\n"  // 1
      "# ### ##### #\n"  // 2
      "# ### ##### #\n"  // 3
      "# ### ##### #\n"  // 4
      "# ### ##### #\n"  // 5
      "# ### ##### #\n"  // 6
      "#           #\n"  // 7
      "########## ##\n"  // 8
      "##########s##\n"  // 9
      "#############\n"; // 10

    AStarMap map;
    map.createMapItemsfromAscii(testMap4);
    map.debugMapItems();

    map.buildNodes();

    AStarPathFinding apf;
    apf.setMap(&map);
    apf.setStart(10, 9);
    apf.setTarget(2, 1);
    apf.findPath();
    apf.debugPath();
    map.debugPath(apf.getPath());

    map.clearNodes();

    //return a.exec();

    return 0;
}


/*
QString testMap2 =
  "#############\n"  // 0
  "#        t ##\n"  // 1
  "# ###########\n"  // 2
  "# ###########\n"  // 3
  "# ###########\n"  // 4
  "# ###########\n"  // 5
  "# ###########\n"  // 6
  "#          ##\n"  // 7
  "########## ##\n"  // 8
  "########## ##\n"  // 9
  "##########s##\n"; // 10


QString testMap4 =
// 0123456789abc
  "#############\n"  // 0
  "# t #      ##\n"  // 1
  "########## ##\n"  // 2
  "# ######## ##\n"  // 3
  "# ######## ##\n"  // 4
  "# ######## ##\n"  // 5
  "# ######## ##\n"  // 6
  "#          ##\n"  // 7
  "########## ##\n"  // 8
  "##########s##\n"  // 9
  "#############\n"; // 10
  */


/*

 case MapItem::Block:
    c = '#';
    break;
 case MapItem::Bomb:
    c = 'B';
    break;
 case MapItem::Extra:
    c = 'E';
    break;
 case MapItem::Stone:
    c = 'S';
    break;
 case MapItem::Unknown:






*/
