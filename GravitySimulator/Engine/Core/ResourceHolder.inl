
template<class Resource, class Identifier>
template<class... Parameters>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename, const Parameters&... parameters)
{
    // Create and load resource
    auto resource = std::make_unique<Resource>();

    if (!resource->loadFromFile(filename, parameters...))
    {
        throw std::runtime_error("ResourceHolder::Load - Failed to load " + filename);
    }

    // If loading successful, insert resource to map
    auto element = m_resourceMap.emplace(id, std::move(resource));
    assert(element.second);
}

template<class Resource, class Identifier>
Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
    // Call const equivalent to avoid code duplication
    return const_cast<Resource&>(static_cast<const ResourceHolder*>(this)->get(id));
}

template<class Resource, class Identifier>
const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
    auto element = m_resourceMap.find(id);
    assert(element != m_resourceMap.end());
    return *element->second;
}