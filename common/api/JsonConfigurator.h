#ifndef CJSONCONFIGURATOR_H_
#define CJSONCONFIGURATOR_H_

#include "CommonType.h"
#include "commondefine.h"

namespace zl {

// json configurator
class CJsonConfigurator
{
public:
    static int32_t Load(
            zl::AppConfig& config,
            const QString& filepath = CONFIG_PATH);
    static int32_t Save(
            const zl::AppConfig& config,
            const QString& filepath = CONFIG_PATH);
};

}

#endif // CJSONCONFIGURATOR_H_
