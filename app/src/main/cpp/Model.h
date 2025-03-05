#ifndef ANDROIDGLINVESTIGATIONS_MODEL_H
#define ANDROIDGLINVESTIGATIONS_MODEL_H

#include <vector>
#include "TextureAsset.h"

class Model {
public:
    inline Model(
            std::vector<float> vertices,
            std::vector<unsigned int> indices,
            std::shared_ptr<TextureAsset> spTexture)
            : vertices_(std::move(vertices)),
              indices_(std::move(indices)),
              spTexture_(std::move(spTexture)) {}

    inline const float* getVertexData() const {
        return vertices_.data();
    }

    inline const int getVertexCount() const {
        return vertices_.size();
    }

    inline const int getIndexCount() const {
        return indices_.size();
    }

    inline const unsigned int * getIndexData() const {
        return indices_.data();
    }

    inline const TextureAsset &getTexture() const {
        return *spTexture_;
    }

private:
    std::vector<float> vertices_;
    std::vector<unsigned int> indices_;
    std::shared_ptr<TextureAsset> spTexture_;
};

#endif //ANDROIDGLINVESTIGATIONS_MODEL_H