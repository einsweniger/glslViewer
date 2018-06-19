//
// Created by bone on 09.03.18.
//

#include "gl/gl.h"
#include "util.h"
#include "BasicInterface.h"
#include <algorithm>

namespace minuseins::interfaces {
    BasicInterface::BasicInterface(GLenum interface, GLuint program, std::vector<GLenum> properties) :
            interface(interface), program(program), properties{properties} {}

    types::property_t BasicInterface::GetProgramResourceiv(const GLuint index, const std::vector<GLenum> &props) const {
        std::vector<GLint> params(props.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
        auto propCount = static_cast<GLsizei>(props.size()); //Values for propCount properties specified by the array props are returned.
        auto bufSize = static_cast<GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
        GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
        glGetProgramResourceiv(program, interface, index, propCount, &props[0], bufSize, &length, &params[0]);
        types::property_t result;

        //result = zip(props, params)
        std::transform(props.begin(), props.end(), params.begin(),
                       std::inserter(result, result.end()),
                       std::make_pair<GLenum const &, GLint const &>);
        return result;
    }

    std::vector<GLint> BasicInterface::GetProgramResourceiv_vector(const GLuint index, const GLenum props,
                                                                  const GLuint size) const {
        std::vector<GLint> params(size);
        const GLsizei propCount = 1;
        GLsizei length;
        auto bufSize = static_cast<GLsizei>(params.size());
        glGetProgramResourceiv(program, interface, index, propCount, &props, bufSize, &length, &params[0]);
        return params;
    }

    std::string BasicInterface::GetProgramResourceName(const GLuint index, const GLint bufSize) const {
        std::string name; // The name string assigned to is returned as a null-terminated string in name.
        GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
        name.resize(bufSize);
        glGetProgramResourceName(program, interface, index, bufSize, &length, &name[0]);
        name.resize(length);
        return name;
    }

    std::string BasicInterface::GetProgramResourceName(const GLuint index) const {
        auto length = GetProgramInterfaceiv(GL_MAX_NAME_LENGTH);
        return GetProgramResourceName(index, length);
    }

    GLuint BasicInterface::GetProgramInterfaceiv(const GLenum property) const {
        GLint result = 0;
        glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);

    }

    GLuint BasicInterface::GetActiveResourceCount() const {
        return GetProgramInterfaceiv(GL_ACTIVE_RESOURCES);
    }

    GLuint BasicInterface::GetMaxNameLenght() const {
        return GetProgramInterfaceiv(GL_MAX_NAME_LENGTH);
    }

    GLuint BasicInterface::GetMaxNumActiveVariables() const {
        return GetProgramInterfaceiv(GL_MAX_NUM_ACTIVE_VARIABLES);
    }

    GLuint BasicInterface::GetMaxNumCompatibleSubroutines() const {
        return GetProgramInterfaceiv(GL_MAX_NUM_COMPATIBLE_SUBROUTINES);
    }

    GLuint BasicInterface::GetProgramResourceIndex(std::string_view name) const {
        return glGetProgramResourceIndex(program, interface, &name[0]);
    }

    types::property_t BasicInterface::GetResourceProperties(GLuint index) const{
        return GetProgramResourceiv(index, properties);
    }

    types::resource BasicInterface::GetResource(GLuint index) const {
        auto props = GetResourceProperties(index);
        return {index, props};
    }

    types::named_resource BasicInterface::GetNamedResource(GLuint index) const {
        auto res = GetResource(index);
        std::string name;
        try{
            name = GetProgramResourceName(index, res.properties.at(GL_NAME_LENGTH));
        } catch (std::out_of_range&) {
            name = "";
        }
        return {name, GetResource(index)};
    }

    std::vector<types::resource> BasicInterface::GetAllResources() const {
        auto result = std::vector<types::resource>{};
        for (auto resourceIndex : util::range(GetActiveResourceCount())) {
            result.emplace_back(GetResource(resourceIndex));
        }
        return result;
    }

    std::vector<types::named_resource> BasicInterface::GetAllNamedResources() const {
        auto result = std::vector<types::named_resource>();
        for (auto resourceIndex : util::range(GetActiveResourceCount())) {
            result.emplace_back(GetNamedResource(resourceIndex));
        }
        return result;
    }

    std::vector<GLuint> BasicInterface::GetCompatibleSubroutines(const GLuint index, const GLuint length) {
        auto unsig = std::vector<GLuint>{};
        for (auto sig : GetProgramResourceiv_vector(index, GL_COMPATIBLE_SUBROUTINES, length)) {
            unsig.push_back(positive(sig));
        }
        return unsig;
    }

    std::vector<GLuint> BasicInterface::GetActiveVariables(const GLuint index, const GLuint length) {
        auto unsig = std::vector<GLuint>{};
        for (auto sig : GetProgramResourceiv_vector(index, GL_ACTIVE_VARIABLES, length)) {
            unsig.push_back(positive(sig));
        }
        return unsig;
    }

}