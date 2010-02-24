#include "phononengine.h"

#include <QTimer>

PhononEngine::PhononEngine()
  : media_object_(new Phonon::MediaObject(this)),
    audio_output_(new Phonon::AudioOutput(Phonon::MusicCategory, this)),
    state_timer_(new QTimer(this))
{
  Phonon::createPath(media_object_, audio_output_);

  connect(media_object_, SIGNAL(finished()), SLOT(PhononFinished()));
  connect(media_object_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(PhononStateChanged(Phonon::State)));

  state_timer_->setSingleShot(true);
  connect(state_timer_, SIGNAL(timeout()), SLOT(StateTimeoutExpired()));
}

PhononEngine::~PhononEngine() {
  delete media_object_;
  delete audio_output_;
}

bool PhononEngine::init() {
  return true;
}

bool PhononEngine::canDecode(const QUrl &url) const {
  // TODO
  return true;
}

bool PhononEngine::load(const QUrl &url, bool stream) {
  media_object_->setCurrentSource(Phonon::MediaSource(url));
  return true;
}

bool PhononEngine::play(uint offset) {
  media_object_->play();
  return true;
}

void PhononEngine::stop() {
  media_object_->stop();
}

void PhononEngine::pause() {
  media_object_->pause();
}

void PhononEngine::unpause() {
  media_object_->play();
}

Engine::State PhononEngine::state() const {
  switch (media_object_->state()) {
    case Phonon::LoadingState:
    case Phonon::PlayingState:
    case Phonon::BufferingState:
      return Engine::Playing;

    case Phonon::PausedState:
      return Engine::Paused;

    case Phonon::StoppedState:
    case Phonon::ErrorState:
    default:
      return Engine::Empty;
  }
}

uint PhononEngine::position() const {
  return media_object_->currentTime();
}

uint PhononEngine::length() const {
  return media_object_->totalTime();
}

void PhononEngine::seek(uint ms) {
  media_object_->seek(ms);
}

void PhononEngine::setVolumeSW(uint) {
  audio_output_->setVolume(qreal(m_volume) / 100.0);
}

void PhononEngine::PhononFinished() {
  emit trackEnded();
}

void PhononEngine::PhononStateChanged(Phonon::State new_state) {
  if (new_state == Phonon::ErrorState) {
    emit error(media_object_->errorString());
  }

  // Don't emit the state change straight away
  state_timer_->start(100);
}

void PhononEngine::StateTimeoutExpired() {
  qDebug() << state();
  emit stateChanged(state());
}
