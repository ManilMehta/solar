#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

#include "simulation/body.h"
#include "simulation/solar_system_data.h"
#include "simulation/physics.h"
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "rendering/trail.h"

// ── Global state for GLFW callbacks ──────────────────────────────────
struct AppState {
    Camera camera;
    std::vector<CelestialBody>* bodies = nullptr;
    int focusIndex = 0;           // 0 = Sun
    bool paused = false;
    int substepsPerFrame = 10;
    double dt = 0.5;              // days per physics step
    double simTime = 0.0;         // elapsed sim days
    bool dragging = false;
    double lastMouseX = 0, lastMouseY = 0;
    int windowW = 1280, windowH = 800;
};
static AppState app;

static void framebufferCallback(GLFWwindow*, int w, int h) {
    app.windowW = w;
    app.windowH = h;
    glViewport(0, 0, w, h);
}

static void scrollCallback(GLFWwindow*, double, double yoff) {
    app.camera.zoom(static_cast<float>(yoff));
}

static void mouseButtonCallback(GLFWwindow* win, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app.dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(win, &app.lastMouseX, &app.lastMouseY);
    }
}

static void cursorPosCallback(GLFWwindow*, double x, double y) {
    if (app.dragging) {
        float dx = static_cast<float>(x - app.lastMouseX);
        float dy = static_cast<float>(y - app.lastMouseY);
        app.camera.rotate(dx, dy);
    }
    app.lastMouseX = x;
    app.lastMouseY = y;
}

static void keyCallback(GLFWwindow* win, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;

    auto& bodies = *app.bodies;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(win, true);
            break;
        case GLFW_KEY_SPACE:
            app.paused = !app.paused;
            break;
        case GLFW_KEY_R:
            app.dt = -app.dt;
            break;
        case GLFW_KEY_EQUAL: // '+' key
        case GLFW_KEY_UP:
            app.substepsPerFrame = std::min(app.substepsPerFrame * 2, 4096);
            break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_DOWN:
            app.substepsPerFrame = std::max(app.substepsPerFrame / 2, 1);
            break;
        case GLFW_KEY_TAB: {
            app.focusIndex = (app.focusIndex + 1) % static_cast<int>(bodies.size());
            break;
        }
        case GLFW_KEY_0: app.focusIndex = 0; break;
        case GLFW_KEY_1: app.focusIndex = (bodies.size() > 1) ? 1 : 0; break;
        case GLFW_KEY_2: app.focusIndex = (bodies.size() > 2) ? 2 : 0; break;
        case GLFW_KEY_3: app.focusIndex = (bodies.size() > 3) ? 3 : 0; break;
        case GLFW_KEY_4: app.focusIndex = (bodies.size() > 5) ? 5 : 0; break; // Mars
        case GLFW_KEY_5: app.focusIndex = (bodies.size() > 8) ? 8 : 0; break; // Jupiter
        case GLFW_KEY_6: app.focusIndex = (bodies.size() > 13) ? 13 : 0; break; // Saturn
        case GLFW_KEY_7: app.focusIndex = (bodies.size() > 15) ? 15 : 0; break; // Uranus
        case GLFW_KEY_8: app.focusIndex = (bodies.size() > 16) ? 16 : 0; break; // Neptune
        default: break;
    }
}

int main() {
    // ── Init GLFW ────────────────────────────────────────────────────
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(app.windowW, app.windowH,
                                           "Solar System Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // ── Load OpenGL ──────────────────────────────────────────────────
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cerr << "Failed to load OpenGL\n";
        return 1;
    }
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version)
              << "." << GLAD_VERSION_MINOR(version) << " loaded\n";

    glEnable(GL_MULTISAMPLE);

    // ── Callbacks ────────────────────────────────────────────────────
    glfwSetFramebufferSizeCallback(window, framebufferCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);

    // get actual framebuffer size (retina)
    glfwGetFramebufferSize(window, &app.windowW, &app.windowH);
    glViewport(0, 0, app.windowW, app.windowH);

    // ── Init simulation ──────────────────────────────────────────────
    auto bodies = createSolarSystem();
    app.bodies = &bodies;

    Physics physics;
    physics.computeAccelerations(bodies); // seed initial accelerations

    // ── Init rendering ───────────────────────────────────────────────
    Renderer renderer;
    renderer.init();

    // Trails: one per body (skip Sun at index 0)
    std::vector<Trail> trails;
    trails.reserve(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i) {
        Trail t(2000);
        t.setColor(bodies[i].color);
        trails.push_back(std::move(t));
    }

    app.camera.distance = 5.0f;

    // ── HUD text buffer ──────────────────────────────────────────────
    double lastFpsTime = glfwGetTime();
    int frameCount = 0;
    double fps = 0.0;

    int trailSampleCounter = 0;

    // ── Main loop ────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Physics substeps
        if (!app.paused) {
            for (int s = 0; s < app.substepsPerFrame; ++s) {
                physics.step(bodies, app.dt);
                app.simTime += app.dt;
            }
        }

        // Update camera focus
        app.camera.setFocus(bodies[app.focusIndex].position);

        // Sample trail points (every few frames to avoid overdraw)
        trailSampleCounter++;
        if (trailSampleCounter >= 3) {
            trailSampleCounter = 0;
            for (size_t i = 0; i < bodies.size(); ++i) {
                glm::vec3 pos(static_cast<float>(bodies[i].position.x),
                              static_cast<float>(bodies[i].position.y),
                              static_cast<float>(bodies[i].position.z));
                trails[i].addPoint(pos);
            }
        }

        // FPS counter
        frameCount++;
        double now = glfwGetTime();
        if (now - lastFpsTime >= 1.0) {
            fps = frameCount / (now - lastFpsTime);
            frameCount = 0;
            lastFpsTime = now;

            // Update window title as HUD
            double years = app.simTime / 365.25;
            char title[256];
            std::snprintf(title, sizeof(title),
                "Solar System | Focus: %s | %.1f days (%.2f yrs) | Speed: %dx | FPS: %.0f%s",
                bodies[app.focusIndex].name.c_str(),
                std::abs(app.simTime), std::abs(years),
                app.substepsPerFrame,
                fps,
                app.paused ? " [PAUSED]" : (app.dt < 0 ? " [REVERSE]" : ""));
            glfwSetWindowTitle(window, title);
        }

        // Render
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(app.windowW) / static_cast<float>(std::max(app.windowH, 1));

        renderer.drawTrails(trails, app.camera, aspect);
        renderer.drawBodies(bodies, app.camera, aspect);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
