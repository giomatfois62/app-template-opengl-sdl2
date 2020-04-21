#ifndef VOLUME_H
#define VOLUME_H

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

inline float lengthSq(const glm::vec3 &vec)
{
    return vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
}

inline float lengthSq(const glm::vec2 &vec)
{
    return vec[0]*vec[0] + vec[1]*vec[1];
}

struct Circle
{
    Circle();
    Circle(const glm::vec2 &center, float radius) :
        center(center),
        radius(radius)
    {
    }

    Circle translated(const glm::vec2 &v)
    {
        return Circle(center + v, radius);
    }

    bool intersect(const glm::vec2 &point)
    {
        return lengthSq(point-center) <= radius*radius;
    }

    bool intersect(const Circle &other)
    {
        float dist = lengthSq(other.center-center);

        return dist <= (other.radius+radius)*(other.radius+radius) &&
                dist >= (other.radius-radius)*(other.radius-radius);
    }

    glm::vec2 center;
    float radius;
};

struct Sphere
{
    Sphere();
    Sphere(const glm::vec3 &center, float radius) :
        center(center),
        radius(radius)
    {
    }

    Sphere translated(const glm::vec3 &v)
    {
        return Sphere(center + v, radius);
    }

    bool intersect(const glm::vec3 &point)
    {
        return lengthSq(point-center) <= radius*radius;
    }

    bool intersect(const Sphere &other)
    {
        float dist = lengthSq(other.center-center);

        return dist <= (other.radius+radius)*(other.radius+radius) &&
                dist >= (other.radius-radius)*(other.radius-radius);
    }

    glm::vec3 center;
    float radius;
};

struct Box
{
    Box() {}

    Box(const glm::vec2 &min, const glm::vec2 &max) :
        min(min), max(max)
    {

    }

    Box translated(const glm::vec2 &point)
    {
        return Box(min + point, max + point);
    }

    bool intersect(const Box &other)
    {
        return (min[0] <= other.max[0] && max[0] >= other.min[0]) &&
                (min[1] <= other.max[1] && max[1] >= other.min[1]);
    }

    bool intersect(const glm::vec2 &point)
    {
        return (point[0] >= min[0] && point.x <= max[0]) &&
                (point[1] >= min[1] && point.y <= max[1]);
    }

    std::vector<Box> subdivide()
    {
        glm::vec2 center = 0.5f * (min + max);

        std::vector<Box> sub;

        sub.push_back(Box(min,center));
        sub.push_back(Box(center, max));
        sub.push_back(Box(min + glm::vec2(0, center[1]-min[1]), center + glm::vec2(0, max[1]-center[1])));
        sub.push_back(Box(center - glm::vec2(0, center[1]-min[1]), max - glm::vec2(0, max[1]-center[1])));

        return sub;
    }

    glm::vec2 min;
    glm::vec2 max;
};


struct Volume
{
    Volume() {}

    Volume(const glm::vec3 &_min, const glm::vec3 &_max) :
        min(_min), max(_max)
    {

    }

    Volume translated(const glm::vec3 &point)
    {
        return Volume(min + point, max + point);
    }

    bool intersect(const Volume &other)
    {
        return (min[0] <= other.max[0] && max[0] >= other.min[0]) &&
                (min[1] <= other.max[1] && max[1] >= other.min[1]) &&
                (min[2] <= other.max[2] && max[2] >= other.min[2]);
    }

    bool intersect(const glm::vec3 &point)
    {
        return (point[0] >= min[0] && point.x <= max[0]) &&
                (point[1] >= min[1] && point.y <= max[1]) &&
                (point[2] >= min[2] && point.z <= max[2]);
    }

    std::vector<Volume> subdivide()
    {
        glm::vec3 center = 0.5f * (min + max);

        std::vector<Volume> sub;

        sub.push_back(Volume(min,center));
        sub.push_back(Volume(center, max));
        sub.push_back(Volume(glm::vec3(center[0], min[1], min[2]), glm::vec3(max[0], center[1], center[2])));
        sub.push_back(Volume(glm::vec3(center[0], min[1], center[2]), glm::vec3(max[0], center[1], max[2])));
        sub.push_back(Volume(glm::vec3(min[0], min[1], center[2]), glm::vec3(center[0], center[1], max[2])));
        sub.push_back(Volume(glm::vec3(min[0], center[1], min[2]), glm::vec3(center[0], max[1], center[2])));
        sub.push_back(Volume(glm::vec3(center[0], center[1], min[2]), glm::vec3(max[0], max[1], center[2])));
        sub.push_back(Volume(glm::vec3(min[0], center[1], center[2]), glm::vec3(center[0], max[1], max[2])));

        return sub;
    }

    friend std::ostream &operator<<(std::ostream& strm, const Volume& v)
    {
        strm << "[" << v.min[0] << "," << v.min[1] << "," << v.min[2] << "]";
        strm << "[" << v.max[0] << "," << v.max[1] << "," << v.max[2] << "]";
        return strm;
    }

    glm::vec3 min;
    glm::vec3 max;
};

#include <set>
#include <map>

template <class T>
struct SPNode {
public:
    SPNode() : volume(T()), depth(0), id(0) {}
    SPNode(const T& volume, unsigned int _depth, unsigned int _id) :
        volume(volume),
        depth(_depth),
        id(_id)
    {
    }

    void print()
    {
        for(size_t i = 0; i < depth; ++i)
            std::cout << "-";
        std::cout << "> Node " << id << " (depth " << depth << ")";

        for(size_t item : items)
            std::cout << item << " ";
        std::cout << "\n";

        for(SPNode<T> &child : children)
            child.print();
    }

    T volume;

    std::vector<SPNode> children;
    std::set<size_t> items;

    unsigned int depth;
    unsigned int id;
};

template <class T>
struct SPItem {
    SPItem() {}
    SPItem(const T& volume) : volume(volume) {}

    T volume;
    std::map<unsigned int, SPNode<T>*> address;
};

template <class T>
class SPTree {
public:
    SPTree() : root(T(), 0, 0) {}
    SPTree(const T& volume, unsigned int maxBinSize, unsigned int maxDepth) :
        root(volume, 0, 0),
        maxBinSize(maxBinSize),
        maxDepth(maxDepth)
    {
    }

    void insert(size_t id, T &volume)
    {
        items[id] = SPItem<T>(volume);
        recursiveInsert(&root, id, volume);
    }

    void remove(size_t id)
    {
        // remove from current nodes
        auto it = items.find(id);
        for (auto& kv : it->second.address)
            kv.second->items.erase(id);

        items.erase(it);
    }

    void update(size_t id, T &volume)
    {
        // remove from current nodes
        auto it = items.find(id);
        for (auto& kv : it->second.address) {
            if(!volume.intersect(kv.second->volume))
                kv.second->items.erase(id);
        }

        insert(id, volume);
    }

    template <class C>
    std::vector<size_t> neighbors(const C &thing)
    {
        std::vector<size_t> list;
        recursiveSearch<C>(&root, thing, list);

        return list;
    }

    void print()
    {
        root.print();
    }

private:
    void recursiveInsert(SPNode<T> *node, size_t id, const T &volume)
    {
        if(!node->volume.intersect(volume))
            return;

        // only insert in leafs
        if(!node->children.size()) {
            node->items.insert(id);
            items[id].address.insert(std::make_pair(node->id, node));
        }

        // try to flush leaf if full
        unsigned int depth = node->depth + 1;

        if(node->items.size() > maxBinSize && depth < maxDepth) {

            std::vector<T> subdiv = node->volume.subdivide();

            for(const T& volume : subdiv)
                node->children.push_back(SPNode<T>(volume, depth, nextNodeId++));

            for(size_t itemId : node->items) {
                SPItem<T> *item = &items[itemId];
                for(SPNode<T> &child : node->children)
                    recursiveInsert(&child, itemId, item->volume);
                item->address.erase(node->id);
            }

            node->items.clear();
            return;
        }

        for(SPNode<T> &child : node->children)
            recursiveInsert(&child, id, volume);
    }

    template <class C>
    void recursiveSearch(SPNode<T> *node, const C &thing, std::vector<size_t> &list)
    {
        if(!node->volume.intersect(thing))
            return;

        std::copy(node->items.begin(), node->items.end(), std::back_inserter(list));
        for(SPNode<T> &child : node->children)
            recursiveSearch<C>(&child, thing, list);
    }

    SPNode<T> root;

    unsigned int nextNodeId = 1;
    unsigned int maxBinSize = 0;
    unsigned int maxDepth = 0;

    std::map<size_t, SPItem<T>> items;
};

typedef SPTree<Volume> Octree;
typedef SPTree<Box> Quadtree;

// TODO: kd-tree, BSP

#endif
