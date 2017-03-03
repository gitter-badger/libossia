#pragma once
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace oscquery
{
namespace detail
{
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, int v)
{ writer.Int(v); }
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, double v)
{ writer.Double(v); }
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, float v)
{ writer.Double(v); }
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, bool v)
{ writer.Bool(v); }
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::string& v)
{ writer.String(v); }
inline void write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, const ossia::value& v)
{ v.apply(value_to_json{writer}); }

//! Write a domain to json.
struct domain_to_json
{
  rapidjson::Writer<rapidjson::StringBuffer>& writer;
  void operator()()
  {
  }

  void operator()(const ossia::domain_base<impulse> & dom)
  {
  }

  template<typename T>
  void operator()(const ossia::domain_base<T> & dom)
  {
    bool has_min = bool(dom.min);
    bool has_max = bool(dom.max);
    bool has_values = !dom.values.empty();
    if(has_min || has_max || has_values)
    {
      writer.StartObject();
      if(has_min)
      {
        writer.Key("MIN");
        write_json(writer, *dom.min);
        writer.Int(*dom.min);
      }
      if(has_max)
      {
        writer.Key("MAX");
        write_json(writer, *dom.max);
      }
      if(has_values)
      {
        writer.Key("VALS");
        writer.StartArray();
        for(auto val : dom.values)
          write_json(writer, val);
        writer.EndArray();
      }
      writer.EndObject();
    }
  }

  void operator()(const ossia::domain_base<std::string>& dom)
  {
    if(!dom.values.empty())
    {
      writer.StartObject();
      writer.Key("VALS");
      writer.StartArray();
      for(auto val : dom.values)
        write_json(writer, val);
      writer.EndArray();
      writer.EndObject();
    }
  }

  void operator()(const ossia::vector_domain & dom)
  {
    // TODO
    /*
    int min_count = dom.min ? dom.min->size() : 0;
    int max_count = dom.max ? dom.max->size() : 0;
    int values_count = dom.values.size();
    int N = std::min(std::min(min_count, max_count), values_count);
    if(N > 0)
    {
      auto min_it = dom.min->begin();
      auto max_it = dom.max->begin();
      auto values_it = dom.values.begin();
      for(int i = 0; i < N; i++)
      {
        writer.StartObject();
        writer.Key("MIN");
        write_json(*min_it);

        writer.Key("MAX");
        write_json(*max_it);

      }
    }
    */

    /*
    for(int i = 0; i < N; i++)
    if(dom.min)
    {
      writer.StartArray();
      for(auto& val : *dom.min)
        val.apply(value_to_json{writer});
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }

    if(dom.max)
    {
      writer.StartArray();
      for(auto& val : *dom.max)
        val.apply(value_to_json{writer});
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }

    if(!dom.values.empty())
    {
      writer.StartArray();
      for(const auto& val : dom.values)
      {
        writer.StartArray();
        for(auto& sub : val)
          sub.apply(value_to_json{writer});
        writer.EndArray();
      }
      writer.EndArray();
    }
    else
    {
      writer.Null(); // TODO why not just nothing ?
    }
    */
  }

  template<std::size_t N>
  void operator()(const ossia::vecf_domain<N> & dom)
  {
    // TODO
    /*
    if(dom.min)
    {
      auto& vec = *dom.min;
      writer.StartArray();
      for(std::size_t i = 0; i < N; i++)
        writer.Double(vec[i]);
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }

    if(dom.max)
    {
      auto& vec = *dom.max;
      writer.StartArray();
      for(std::size_t i = 0; i < N; i++)
        writer.Double(vec[i]);
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }

    if(!dom.values.empty())
    {
      writer.StartArray();
      for(const auto& vec : dom.values)
      {
        writer.StartArray();
        for(std::size_t i = 0; i < N; i++)
          writer.Double(vec[i]);
        writer.EndArray();
      }
      writer.EndArray();
    }
    else
    {
      writer.Null(); // TODO why not just nothing ?
    }
    */
  }
  void operator()(const ossia::domain_base<ossia::value> & dom)
  {
    if(dom.min)
    {
      dom.min->apply(value_to_json{writer});
    }
    else
    {
      writer.Null();
    }

    if(dom.max)
    {
      dom.max->apply(value_to_json{writer});
    }
    else
    {
      writer.Null();
    }

    if(!dom.values.empty())
    {
      writer.StartArray();
      for(const auto& val: dom.values)
      {
        val.apply(value_to_json{writer});
      }
      writer.EndArray();
    }
    else
    {
      writer.Null(); // TODO why not just nothing ?
    }
  }
};


/*
struct json_to_domain
{
    const rapidjson::Value& value; // Is an array
    bool operator()()
    {
      return false;
    }

    bool operator()(ossia::domain_base<impulse> & dom)
    {
      return true;
    }

    bool operator()(ossia::domain_base<int32_t> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsInt()) dom.min = arr[0].GetInt(); else dom.min = ossia::none;
          if(arr[1].IsInt()) dom.max = arr[1].GetInt(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsInt())
                dom.values.insert(sub.GetInt());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<float> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsNumber()) dom.min = arr[0].GetDouble(); else dom.min = ossia::none;
          if(arr[1].IsNumber()) dom.max = arr[1].GetDouble(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsNumber())
                dom.values.insert(sub.GetDouble());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<char> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsString() && arr[0].GetStringLength() > 0) dom.min = arr[0].GetString()[0]; else dom.min = ossia::none;
          if(arr[1].IsString() && arr[1].GetStringLength() > 0) dom.max = arr[1].GetString()[0]; else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsString() && arr[0].GetStringLength() > 0)
                dom.values.insert(sub.GetString()[0]);
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<bool> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsBool()) dom.min = arr[0].GetBool(); else dom.min = ossia::none;
          if(arr[1].IsBool()) dom.max = arr[1].GetBool(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsBool())
                dom.values.insert(sub.GetBool());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<std::string> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsString())
                dom.values.insert(sub.GetString());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::vector_domain & dom)
    {
      // Todo : why not instead : RANGE: { min: .. max: .. } or RANGE: [ 0, 4, 6, 123 ]... this would allow more possible domains in the future

      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 3)
        {
          if(arr[0].IsArray())
          {
            // Read the array freely



          }
        }
      }
      return false;
    }

    template<std::size_t N>
    bool operator()(ossia::vecf_domain<N> & dom)
    {
      // TODO
        if(dom.min)
        {
            auto& vec = *dom.min;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            auto& vec = *dom.max;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& vec : dom.values)
            {
                writer.StartArray();
                for(std::size_t i = 0; i < N; i++)
                    writer.Double(vec[i]);
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
      return false;
    }

    bool operator()(ossia::domain_base<ossia::value> & dom)
    {
      // TODO
        if(dom.min)
        {
            dom.min->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            dom.max->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val: dom.values)
            {
                val.apply(value_to_json{writer});
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
      return false;
    }
};
*/
}
}
}
