#pragma once

#include <cassert> // Used in ResourceHolder.inl
#include <memory>
#include <unordered_map>

template<class Resource, class Identifier>
class ResourceHolder
{
public:
    template<class... Parameters>
    void load(Identifier id, const std::string& filename, const Parameters&... parameters);

    Resource& get(Identifier id);
    const Resource& get(Identifier id) const;
private:
    std::unordered_map<Identifier, std::unique_ptr<Resource>> m_resourceMap;
};

#include "ResourceHolder.inl"