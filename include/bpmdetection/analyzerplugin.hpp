#ifndef ANALYZERPLUGIN_HPP_
#define ANALYZERPLUGIN_HPP_

#include <string>
#include <vector>

class AnalyzerPluginInfo {
 public:
  AnalyzerPluginInfo(const std::string& id,
                     const std::string& author,
        const std::string& name,
        bool isConstantTempoSupported)
    : m_id(id)
    , m_author(author)
    , m_name(name)
    , m_isConstantTempoSupported(isConstantTempoSupported)
  {}

  const std::string& id() const {
    return m_id;
  }

  const std::string& author() const {
    return m_author;
  }

  const std::string& name() const {
    return m_name;
  }

  bool isConstantTempoSupported() const {
    return m_isConstantTempoSupported;
  }

 private:
  std::string m_id;
  std::string m_author;
  std::string m_name;
  bool m_isConstantTempoSupported;
};

class AnalyzerPlugin {
 public:
  virtual ~AnalyzerPlugin() = default;

  virtual std::string id() const {
    return info().id();
  }
  virtual std::string author() const {
    return info().author();
  }
  virtual std::string name() const {
    return info().name();
  }
  virtual AnalyzerPluginInfo info() const = 0;

  virtual bool initialize(unsigned int sampleRate) = 0;
  virtual bool processSamples(const float* pIn, const int iLen) = 0;
  virtual bool finalize() = 0;
};

class AnalyzerBeatsPlugin : public AnalyzerPlugin {
 public:
  ~AnalyzerBeatsPlugin() override = default;

  virtual bool supportsBeatTracking() const = 0;
  virtual double getBpm() const {
    return 0.0;
  }
  virtual std::vector<unsigned int> getBeats() const {
    return {};
  }
};

#endif // ANALYZERPLUGIN_HPP_
