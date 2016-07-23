#include "Editor/Expression_impl.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Expression> Expression::create(bool result)
  {
    return make_shared<impl::JamomaExpression>(result);
  }

  Expression::~Expression() = default;
}


