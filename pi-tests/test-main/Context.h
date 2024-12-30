/**
 * @file Context.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TMAIN_CONTEXT_H_
#define TMAIN_CONTEXT_H_

#include "Environment.h"

namespace tmain {

class Context : public smachine::env::Environment {
public:
    Context(){}
    virtual ~Context() noexcept {}

    const std::string name() const {
        return std::string("Context is my name");
    }
};

}

#endif
