//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

namespace xe {

    class Material {
    public:
        virtual void bind() = 0;

        virtual void unbind() {}
    };
}
