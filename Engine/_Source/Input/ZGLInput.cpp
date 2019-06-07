/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZGLInput.cpp
 
    Created by Adrian Sanchez on 28/01/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.
 
 This file is part of Zenith.
 
 Zenith is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Zenith is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ZGLInput.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectLookEvent.hpp"
#include "ZObjectDragEvent.hpp"
#include "ZObjectMoveEvent.hpp"
#include "ZQuitEvent.hpp"
#include "ZFireEvent.hpp"

unsigned int ZGLInput::keyMap_[ZKEY_LAST+1];
unsigned int ZGLInput::mouseMap_[ZMOUSE_LAST+1];

void ZGLInput::Initialize() {
    ZInput::Initialize();
    
    keyMap_[GLFW_KEY_SPACE] = ZKEY_SPACE;
    keyMap_[GLFW_KEY_APOSTROPHE] = ZKEY_APOSTROPHE;
    keyMap_[GLFW_KEY_COMMA] = ZKEY_COMMA;
    keyMap_[GLFW_KEY_MINUS] = ZKEY_MINUS;
    keyMap_[GLFW_KEY_PERIOD] = ZKEY_PERIOD;
    keyMap_[GLFW_KEY_SLASH] = ZKEY_SLASH;
    keyMap_[GLFW_KEY_0] = ZKEY_0;
    keyMap_[GLFW_KEY_1] = ZKEY_1;
    keyMap_[GLFW_KEY_2] = ZKEY_2;
    keyMap_[GLFW_KEY_3] = ZKEY_3;
    keyMap_[GLFW_KEY_4] = ZKEY_4;
    keyMap_[GLFW_KEY_5] = ZKEY_5;
    keyMap_[GLFW_KEY_6] = ZKEY_6;
    keyMap_[GLFW_KEY_7] = ZKEY_7;
    keyMap_[GLFW_KEY_8] = ZKEY_8;
    keyMap_[GLFW_KEY_9] = ZKEY_9;
    keyMap_[GLFW_KEY_SEMICOLON] = ZKEY_SEMICOLON;
    keyMap_[GLFW_KEY_EQUAL] = ZKEY_EQUAL;
    keyMap_[GLFW_KEY_A] = ZKEY_A;
    keyMap_[GLFW_KEY_B] = ZKEY_B;
    keyMap_[GLFW_KEY_C] = ZKEY_C;
    keyMap_[GLFW_KEY_D] = ZKEY_D;
    keyMap_[GLFW_KEY_E] = ZKEY_E;
    keyMap_[GLFW_KEY_F] = ZKEY_F;
    keyMap_[GLFW_KEY_G] = ZKEY_G;
    keyMap_[GLFW_KEY_H] = ZKEY_H;
    keyMap_[GLFW_KEY_I] = ZKEY_I;
    keyMap_[GLFW_KEY_J] = ZKEY_J;
    keyMap_[GLFW_KEY_K] = ZKEY_K;
    keyMap_[GLFW_KEY_L] = ZKEY_L;
    keyMap_[GLFW_KEY_M] = ZKEY_M;
    keyMap_[GLFW_KEY_N] = ZKEY_N;
    keyMap_[GLFW_KEY_O] = ZKEY_O;
    keyMap_[GLFW_KEY_P] = ZKEY_P;
    keyMap_[GLFW_KEY_Q] = ZKEY_Q;
    keyMap_[GLFW_KEY_R] = ZKEY_R;
    keyMap_[GLFW_KEY_S] = ZKEY_S;
    keyMap_[GLFW_KEY_T] = ZKEY_T;
    keyMap_[GLFW_KEY_U] = ZKEY_U;
    keyMap_[GLFW_KEY_V] = ZKEY_V;
    keyMap_[GLFW_KEY_W] = ZKEY_W;
    keyMap_[GLFW_KEY_X] = ZKEY_X;
    keyMap_[GLFW_KEY_Y] = ZKEY_Y;
    keyMap_[GLFW_KEY_Z] = ZKEY_Z;
    keyMap_[GLFW_KEY_LEFT_BRACKET] = ZKEY_LEFT_BRACKET;
    keyMap_[GLFW_KEY_BACKSLASH] = ZKEY_BACKSLASH;
    keyMap_[GLFW_KEY_RIGHT_BRACKET] = ZKEY_RIGHT_BRACKET;
    keyMap_[GLFW_KEY_GRAVE_ACCENT] = ZKEY_GRAVE_ACCENT;
    keyMap_[GLFW_KEY_ESCAPE] = ZKEY_ESCAPE;
    keyMap_[GLFW_KEY_ENTER] = ZKEY_ENTER;
    keyMap_[GLFW_KEY_TAB] = ZKEY_TAB;
    keyMap_[GLFW_KEY_BACKSPACE] = ZKEY_BACKSPACE;
    keyMap_[GLFW_KEY_INSERT] = ZKEY_INSERT;
    keyMap_[GLFW_KEY_DELETE] = ZKEY_DELETE;
    keyMap_[GLFW_KEY_RIGHT] = ZKEY_RIGHT;
    keyMap_[GLFW_KEY_LEFT] = ZKEY_LEFT;
    keyMap_[GLFW_KEY_DOWN] = ZKEY_DOWN;
    keyMap_[GLFW_KEY_UP] = ZKEY_UP;
    keyMap_[GLFW_KEY_PAGE_UP] = ZKEY_PAGE_UP;
    keyMap_[GLFW_KEY_PAGE_DOWN] = ZKEY_PAGE_DOWN;
    keyMap_[GLFW_KEY_HOME] = ZKEY_HOME;
    keyMap_[GLFW_KEY_END] = ZKEY_END;
    keyMap_[GLFW_KEY_CAPS_LOCK] = ZKEY_CAPS_LOCK;
    keyMap_[GLFW_KEY_SCROLL_LOCK] = ZKEY_SCROLL_LOCK;
    keyMap_[GLFW_KEY_NUM_LOCK] = ZKEY_NUM_LOCK;
    keyMap_[GLFW_KEY_PRINT_SCREEN] = ZKEY_PRINT_SCREEN;
    keyMap_[GLFW_KEY_PAUSE] = ZKEY_PAUSE;
    keyMap_[GLFW_KEY_F1] = ZKEY_F1;
    keyMap_[GLFW_KEY_F2] = ZKEY_F2;
    keyMap_[GLFW_KEY_F3] = ZKEY_F3;
    keyMap_[GLFW_KEY_F4] = ZKEY_F4;
    keyMap_[GLFW_KEY_F5] = ZKEY_F5;
    keyMap_[GLFW_KEY_F6] = ZKEY_F6;
    keyMap_[GLFW_KEY_F7] = ZKEY_F7;
    keyMap_[GLFW_KEY_F8] = ZKEY_F8;
    keyMap_[GLFW_KEY_F9] = ZKEY_F9;
    keyMap_[GLFW_KEY_F10] = ZKEY_F10;
    keyMap_[GLFW_KEY_F11] = ZKEY_F11;
    keyMap_[GLFW_KEY_F12] = ZKEY_F12;
    keyMap_[GLFW_KEY_F13] = ZKEY_F13;
    keyMap_[GLFW_KEY_F14] = ZKEY_F14;
    keyMap_[GLFW_KEY_F15] = ZKEY_F15;
    keyMap_[GLFW_KEY_F16] = ZKEY_F16;
    keyMap_[GLFW_KEY_F17] = ZKEY_F17;
    keyMap_[GLFW_KEY_F18] = ZKEY_F18;
    keyMap_[GLFW_KEY_F19] = ZKEY_F19;
    keyMap_[GLFW_KEY_F20] = ZKEY_F20;
    keyMap_[GLFW_KEY_F21] = ZKEY_F21;
    keyMap_[GLFW_KEY_F22] = ZKEY_F22;
    keyMap_[GLFW_KEY_F23] = ZKEY_F23;
    keyMap_[GLFW_KEY_F24] = ZKEY_F24;
    keyMap_[GLFW_KEY_F25] = ZKEY_F25;
    keyMap_[GLFW_KEY_KP_0] = ZKEY_KP_0;
    keyMap_[GLFW_KEY_KP_1] = ZKEY_KP_1;
    keyMap_[GLFW_KEY_KP_2] = ZKEY_KP_2;
    keyMap_[GLFW_KEY_KP_3] = ZKEY_KP_3;
    keyMap_[GLFW_KEY_KP_4] = ZKEY_KP_4;
    keyMap_[GLFW_KEY_KP_5] = ZKEY_KP_5;
    keyMap_[GLFW_KEY_KP_6] = ZKEY_KP_6;
    keyMap_[GLFW_KEY_KP_7] = ZKEY_KP_7;
    keyMap_[GLFW_KEY_KP_8] = ZKEY_KP_8;
    keyMap_[GLFW_KEY_KP_9] = ZKEY_KP_9;
    keyMap_[GLFW_KEY_KP_DECIMAL] = ZKEY_KP_DECIMAL;
    keyMap_[GLFW_KEY_KP_DIVIDE] = ZKEY_KP_DIVIDE;
    keyMap_[GLFW_KEY_KP_MULTIPLY] = ZKEY_KP_MULTIPLY;
    keyMap_[GLFW_KEY_KP_SUBTRACT] = ZKEY_KP_SUBTRACT;
    keyMap_[GLFW_KEY_KP_ADD] = ZKEY_KP_ADD;
    keyMap_[GLFW_KEY_KP_ENTER] = ZKEY_KP_ENTER;
    keyMap_[GLFW_KEY_KP_EQUAL] = ZKEY_KP_EQUAL;
    keyMap_[GLFW_KEY_LEFT_SHIFT] = ZKEY_LEFT_SHIFT;
    keyMap_[GLFW_KEY_LEFT_CONTROL] = ZKEY_LEFT_CONTROL;
    keyMap_[GLFW_KEY_LEFT_ALT] = ZKEY_LEFT_ALT;
    keyMap_[GLFW_KEY_LEFT_SUPER] = ZKEY_LEFT_SUPER;
    keyMap_[GLFW_KEY_RIGHT_SHIFT] = ZKEY_RIGHT_SHIFT;
    keyMap_[GLFW_KEY_RIGHT_CONTROL] = ZKEY_RIGHT_CONTROL;
    keyMap_[GLFW_KEY_RIGHT_ALT] = ZKEY_RIGHT_ALT;
    keyMap_[GLFW_KEY_RIGHT_SUPER] = ZKEY_RIGHT_SUPER;
    keyMap_[GLFW_KEY_MENU] = ZKEY_MENU;
    
    mouseMap_[GLFW_MOUSE_BUTTON_LEFT] = ZMOUSE_LEFT;
    mouseMap_[GLFW_MOUSE_BUTTON_RIGHT] = ZMOUSE_RIGHT;
    mouseMap_[GLFW_MOUSE_BUTTON_MIDDLE] = ZMOUSE_MIDDLE;
    mouseMap_[GLFW_MOUSE_BUTTON_4] = ZMOUSE_4;
    mouseMap_[GLFW_MOUSE_BUTTON_5] = ZMOUSE_5;
    mouseMap_[GLFW_MOUSE_BUTTON_6] = ZMOUSE_6;
    mouseMap_[GLFW_MOUSE_BUTTON_7] = ZMOUSE_7;
    mouseMap_[GLFW_MOUSE_BUTTON_8] = ZMOUSE_8;
    mouseMap_[GLFW_MOUSE_BUTTON_LAST] = ZMOUSE_LAST;
    
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwSetKeyCallback(windowHandle, &ZGLInput::KeyCallback);
    glfwSetMouseButtonCallback(windowHandle, &ZGLInput::MouseButtonCallback);
}

void ZGLInput::Update() {
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    
    if (keyPress_[ZKEY_W]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, 1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKEY_A]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(-1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKEY_S]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, -1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKEY_D]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKEY_ESCAPE]) {
        std::shared_ptr<ZQuitEvent> quitEvent(new ZQuitEvent);
        zenith::EventAgent()->TriggerEvent(quitEvent);
    }
    
    double yaw, pitch;
    glfwGetCursorPos(windowHandle, &yaw, &pitch);
    
    if (firstLook_) {
        lastYaw_ = yaw; lastPitch_ = pitch;
        firstLook_ = false;
    }
    
    double deltaYaw = yaw - lastYaw_;
    double deltaPitch = lastPitch_ - pitch;
    
    if (mousePress_[ZMOUSE_LEFT]) {
        if (deltaYaw != 0.0 || deltaPitch != 0.0) {
            std::shared_ptr<ZObjectDragEvent> dragEvent(new ZObjectDragEvent(deltaYaw, deltaPitch, 0.f));
            zenith::EventAgent()->TriggerEvent(dragEvent);
        } else {
            std::shared_ptr<ZFireEvent> fireEvent(new ZFireEvent(yaw, pitch, 0.f));
            zenith::EventAgent()->TriggerEvent(fireEvent);
        }
    }
    
    if (deltaYaw != 0.0 || deltaPitch != 0.0) {
        std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(deltaYaw, deltaPitch));
        zenith::EventAgent()->TriggerEvent(lookEvent);
    }
    
    lastYaw_ = yaw; lastPitch_ = pitch;
}

void ZGLInput::GetCursorPosition(double& x, double& y) {
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwGetCursorPos(windowHandle, &x, &y);
}

void ZGLInput::SetCursorPosition(double& x, double& y) {
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwSetCursorPos(windowHandle, x, y);
}

void ZGLInput::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    zenith::Input()->SetKey(keyMap_[key], action != GLFW_RELEASE);
}

void ZGLInput::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    zenith::Input()->SetMouse(mouseMap_[button], action == GLFW_PRESS);
}
