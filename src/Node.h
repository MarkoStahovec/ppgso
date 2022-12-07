//
// Created by rszar on 6. 12. 2022.
//

#ifndef PPGSO_NODE_H
#define PPGSO_NODE_H
#include "Renderable.h"

class Node {
public:
    Node(std::unique_ptr<Renderable> obj);

    std::unique_ptr<Renderable> object;

    std::list<Node> children;

    void update();

};


#endif //PPGSO_NODE_H
