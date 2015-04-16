#ifndef SCENES_MAIN_SCENE_H_
#define SCENES_MAIN_SCENE_H_

#include "../engine/misc.h"
#include "../engine/scene.h"
#include "../engine/camera.h"
#include "../engine/behaviour.h"
#include "../engine/debug/debug_shape.h"
#include "../engine/gui/label.h"
#include "../engine/cdlod/texture/tex_quad_tree.h"
#include "../loading_screen.h"
#include "../skybox.h"
#include "../terrain.h"
#include "../fps_display.h"

class MainScene : public engine::Scene {
 public:
  MainScene() {
    #if !ENGINE_NO_FULLSCREEN
      glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    #endif

    LoadingScreen();
    glfwSwapBuffers(window());

    auto skybox = addComponent<Skybox>();
    skybox->set_group(-1);
    addComponent<Terrain>();
    int radius = engine::GlobalHeightMap::sphere_radius;
    tp_camera_ = addComponent<engine::ThirdPersonalCamera>(
        M_PI/3, 2, 3*radius, glm::vec3(-radius, 0, 0),
        0.2, 0.1, 0.01, 1.5, radius);

    set_camera(tp_camera_);
    addComponent<engine::gui::Label>(L"FPS: ", glm::vec2{0.8f, 0.9f},
      engine::gui::Font{"src/resources/fonts/Vera.ttf", 30,
      glm::vec4(1, 0, 0, 1)});
    auto fps = addComponent<FpsDisplay>();
    fps->set_group(1);
  }
  private:
    engine::FreeFlyCamera* free_fly_camera_ = nullptr;
    engine::ThirdPersonalCamera* tp_camera_ = nullptr;

  virtual void keyAction(int key, int scancode, int action, int mods) override {
    int radius = engine::GlobalHeightMap::sphere_radius;
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_SPACE) {
        if (free_fly_camera_) {
          tp_camera_ =
            addComponent<engine::ThirdPersonalCamera>(
              M_PI/3, 2, 3*radius, free_fly_camera_->transform()->pos(),
              0.2, 0.1, 0.01, 1.5, radius);
          removeComponent(free_fly_camera_);
          free_fly_camera_ = nullptr;
          set_camera(tp_camera_);
        } else {
          free_fly_camera_ = addComponent<engine::FreeFlyCamera>(
            M_PI/3, 2, 3*radius, tp_camera_->transform()->pos(), glm::vec3(), 50);
          glm::vec3 up{glm::normalize(free_fly_camera_->transform()->pos())};
          free_fly_camera_->transform()->set_forward(glm::cross({1, 0, 0}, up));
          free_fly_camera_->transform()->set_up(up);
          removeComponent(tp_camera_);
          tp_camera_ = nullptr;
          set_camera(free_fly_camera_);
        }
      }
    }
  }

  virtual void update() override {
    if (free_fly_camera_) {
      auto t = free_fly_camera_->transform();
      auto fwd = t->forward();
      glm::vec3 new_up = glm::normalize(t->pos());
      t->set_up(new_up);
      t->set_forward(fwd);
    }
  }

  virtual void mouseScrolled(double xoffset, double yoffset) override {
    if (free_fly_camera_) {
      auto cam = free_fly_camera_;
      cam->set_speed_per_sec(cam->speed_per_sec() * (yoffset > 0 ? 1.1 : 0.9));
    }
  }
};

#endif
