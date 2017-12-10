#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include <ossia-max/src/object_base.hpp>

#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/detail/safe_vec.hpp>
#include <ossia-max_export.h>

#include "attribute.hpp"
#include "parameter.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "device.hpp"
#include "client.hpp"
#include "ossia_object.hpp"
#include "logger.hpp"

extern "C"
{
    OSSIA_MAX_EXPORT void ossia_attribute_setup();
    OSSIA_MAX_EXPORT void ossia_client_setup();
    OSSIA_MAX_EXPORT void ossia_device_setup();
    OSSIA_MAX_EXPORT void ossia_logger_setup();
    OSSIA_MAX_EXPORT void ossia_model_setup();
    OSSIA_MAX_EXPORT void ossia_parameter_setup();
    OSSIA_MAX_EXPORT void ossia_remote_setup();
    OSSIA_MAX_EXPORT void ossia_view_setup();
    OSSIA_MAX_EXPORT void ossia_ossia_setup();
}

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark Library

class ossia_max
{
public:
  static ossia_max& instance();
  static ossia::net::generic_device* get_default_device()
  {
    return &instance().m_device;
  }

  template<typename T>
  t_class* get_class() {
    if(std::is_same<T, parameter>::value) return ossia_parameter_class;
    if(std::is_same<T, remote>::value) return ossia_remote_class;
    if(std::is_same<T, model>::value) return ossia_model_class;
    if(std::is_same<T, view>::value) return ossia_view_class;
    if(std::is_same<T, device>::value) return ossia_device_class;
    if(std::is_same<T, client>::value) return ossia_client_class;
    if(std::is_same<T, attribute>::value) return ossia_attribute_class;
    if(std::is_same<T, ossia_object>::value) return ossia_ossia_class;
    if(std::is_same<T, ossia::max::logger>::value) return ossia_logger_class;
    return nullptr;
  }

  t_class* ossia_client_class{};
  t_class* ossia_attribute_class{};
  t_class* ossia_device_class{};
  t_class* ossia_logger_class{};
  t_class* ossia_model_class{};
  t_class* ossia_parameter_class{};
  t_class* ossia_remote_class{};
  t_class* ossia_view_class{};
  t_class* ossia_ossia_class{};

  // keep list of all objects
  ossia::safe_vector<parameter*> parameters;
  ossia::safe_vector<remote*> remotes;
  ossia::safe_vector<model*> models;
  ossia::safe_vector<view*> views;
  ossia::safe_vector<device*> devices;
  ossia::safe_vector<client*> clients;
  ossia::safe_vector<attribute*> attributes;

  ossia::safe_set<model*> model_quarantine;
  ossia::safe_set<view*> view_quarantine;
  ossia::safe_set<parameter*> parameter_quarantine;
  ossia::safe_set<remote*> remote_quarantine;
  ossia::safe_set<attribute*> attribute_quarantine;

  bool must_register{false};

  static void start_registration(device* x, t_symbol*, long argc, t_atom* argv);
  static void end_registration(device* x, t_symbol*, long argc, t_atom* argv);
private:
  ossia_max();
  ~ossia_max();

  ossia::net::local_protocol* m_localProtocol{};
  ossia::net::generic_device m_device;
  string_map<std::shared_ptr<ossia::websocket_threaded_connection>> m_connections;
};

#pragma mark -
#pragma mark Templates

template <typename T>
extern void object_quarantining(T*);

template <typename T>
extern void object_dequarantining(T*);

template <typename T>
extern bool object_is_quarantined(T*);

struct object_base;

void object_namespace(object_base* x);

#pragma mark -
#pragma mark Utilities

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

/**
 * @brief             Find the first box of classname beside or above (in a
 * parent patcher) context.
 * @details           The function iterate all objects at the same level or
 * above x and return the first instance of classname found.
 * @param object      The Max object instance around which to search.
 * @param classname   The class name of the box object we are looking for.
 * @param start_level Level above current object where to start. 0 for current
 * patcher, 1 start searching in parent canvas.
 * @param level       Return level of the found object
 * @return The instance of the parent box if exists. Otherwise returns nullptr.
 */
object_base* find_parent_box(
    t_object* object, t_symbol* classname, int start_level, int* level);

/**
 * @brief find_parent_box_alive
 * @details Find a parent that is not being removed soon
 * @param object      The Max object instance around which to search.
 * @param classname
 * @param start_level
 * @return
 */
object_base* find_parent_box_alive(
    t_object* object, t_symbol* classname, int start_level, int* level);

/**
 * @brief The box_hierachy class
 * @details Little class to store object pointer and hierarchy level, useful
 * for iterating object from top to bottom.
 */
class box_hierachy
{
public:
  t_object* box;
  int hierarchy;
  t_symbol* classname;

  friend bool operator<(box_hierachy a, box_hierachy b)
  {
    return a.hierarchy < b.hierarchy;
  }
};

/**
 * @brief Find all objects [classname] in the current patcher.
 * @param patcher : patcher in which we are looking for objects
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<object_base*> find_children_to_register(
    t_object* object, t_object* patcher, t_symbol* classname, bool* found_dev = nullptr);

/**
 * @brief             Convenient method to easily get the patcher where a box
 * is
 */
t_object* get_patcher(t_object* object);

/**
 * @brief return relative path to corresponding object
 * @param x
 * @param classname
 * @param found_obj
 * @param address
 * @return
 */
//    bool get_relative_path(t_object* x, t_symbol* classname, t_class**
//    found_obj, std::string& address);

/**
 */
std::vector<std::string> parse_tags_symbol(t_symbol** tags_symbol, long size);

template<typename T, typename... Args>
T* make_ossia(Args&&... args)
{
  auto obj = object_alloc(ossia_max::instance().get_class<T>());
  if(obj)
  {
    t_object tmp;
    memcpy(&tmp, obj, sizeof(t_object));
    auto x = new(obj) T{std::forward<Args>(args)...};
    memcpy(x, &tmp, sizeof(t_object));

    return x;
  }
  return nullptr;
}

} // max namespace
} // ossia namespace
