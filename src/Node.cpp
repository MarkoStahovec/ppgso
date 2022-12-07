//
// Created by rszar on 6. 12. 2022.
//

#include "Node.h"

Node::Node(std::unique_ptr<Renderable> obj) {
    object = std::move(obj);
}

void Node::update() {

}
