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

std::map<unsigned int, ZKey> ZGLInput::keyMap_;
std::map<unsigned int, ZMouse> ZGLInput::mouseMap_;

void ZGLInput::Initialize()
{
    ZInput::Initialize();

    keyMap_[GLFW_KEY_SPACE] = ZKey::SPACE;
    keyMap_[GLFW_KEY_APOSTROPHE] = ZKey::APOSTROPHE;
    keyMap_[GLFW_KEY_COMMA] = ZKey::COMMA;
    keyMap_[GLFW_KEY_MINUS] = ZKey::MINUS;
    keyMap_[GLFW_KEY_PERIOD] = ZKey::PERIOD;
    keyMap_[GLFW_KEY_SLASH] = ZKey::SLASH;
    keyMap_[GLFW_KEY_0] = ZKey::N0;
    keyMap_[GLFW_KEY_1] = ZKey::N1;
    keyMap_[GLFW_KEY_2] = ZKey::N2;
    keyMap_[GLFW_KEY_3] = ZKey::N3;
    keyMap_[GLFW_KEY_4] = ZKey::N4;
    keyMap_[GLFW_KEY_5] = ZKey::N5;
    keyMap_[GLFW_KEY_6] = ZKey::N6;
    keyMap_[GLFW_KEY_7] = ZKey::N7;
    keyMap_[GLFW_KEY_8] = ZKey::N8;
    keyMap_[GLFW_KEY_9] = ZKey::N9;
    keyMap_[GLFW_KEY_SEMICOLON] = ZKey::SEMICOLON;
    keyMap_[GLFW_KEY_EQUAL] = ZKey::EQUAL;
    keyMap_[GLFW_KEY_A] = ZKey::A;
    keyMap_[GLFW_KEY_B] = ZKey::B;
    keyMap_[GLFW_KEY_C] = ZKey::C;
    keyMap_[GLFW_KEY_D] = ZKey::D;
    keyMap_[GLFW_KEY_E] = ZKey::E;
    keyMap_[GLFW_KEY_F] = ZKey::F;
    keyMap_[GLFW_KEY_G] = ZKey::G;
    keyMap_[GLFW_KEY_H] = ZKey::H;
    keyMap_[GLFW_KEY_I] = ZKey::I;
    keyMap_[GLFW_KEY_J] = ZKey::J;
    keyMap_[GLFW_KEY_K] = ZKey::K;
    keyMap_[GLFW_KEY_L] = ZKey::L;
    keyMap_[GLFW_KEY_M] = ZKey::M;
    keyMap_[GLFW_KEY_N] = ZKey::N;
    keyMap_[GLFW_KEY_O] = ZKey::O;
    keyMap_[GLFW_KEY_P] = ZKey::P;
    keyMap_[GLFW_KEY_Q] = ZKey::Q;
    keyMap_[GLFW_KEY_R] = ZKey::R;
    keyMap_[GLFW_KEY_S] = ZKey::S;
    keyMap_[GLFW_KEY_T] = ZKey::T;
    keyMap_[GLFW_KEY_U] = ZKey::U;
    keyMap_[GLFW_KEY_V] = ZKey::V;
    keyMap_[GLFW_KEY_W] = ZKey::W;
    keyMap_[GLFW_KEY_X] = ZKey::X;
    keyMap_[GLFW_KEY_Y] = ZKey::Y;
    keyMap_[GLFW_KEY_Z] = ZKey::Z;
    keyMap_[GLFW_KEY_LEFT_BRACKET] = ZKey::LEFT_BRACKET;
    keyMap_[GLFW_KEY_BACKSLASH] = ZKey::BACKSLASH;
    keyMap_[GLFW_KEY_RIGHT_BRACKET] = ZKey::RIGHT_BRACKET;
    keyMap_[GLFW_KEY_GRAVE_ACCENT] = ZKey::GRAVE_ACCENT;
    keyMap_[GLFW_KEY_ESCAPE] = ZKey::ESCAPE;
    keyMap_[GLFW_KEY_ENTER] = ZKey::ENTER;
    keyMap_[GLFW_KEY_TAB] = ZKey::TAB;
    keyMap_[GLFW_KEY_BACKSPACE] = ZKey::BACKSPACE;
    keyMap_[GLFW_KEY_INSERT] = ZKey::INSERT;
    keyMap_[GLFW_KEY_DELETE] = ZKey::DELETE;
    keyMap_[GLFW_KEY_RIGHT] = ZKey::RIGHT;
    keyMap_[GLFW_KEY_LEFT] = ZKey::LEFT;
    keyMap_[GLFW_KEY_DOWN] = ZKey::DOWN;
    keyMap_[GLFW_KEY_UP] = ZKey::UP;
    keyMap_[GLFW_KEY_PAGE_UP] = ZKey::PAGE_UP;
    keyMap_[GLFW_KEY_PAGE_DOWN] = ZKey::PAGE_DOWN;
    keyMap_[GLFW_KEY_HOME] = ZKey::HOME;
    keyMap_[GLFW_KEY_END] = ZKey::END;
    keyMap_[GLFW_KEY_CAPS_LOCK] = ZKey::CAPS_LOCK;
    keyMap_[GLFW_KEY_SCROLL_LOCK] = ZKey::SCROLL_LOCK;
    keyMap_[GLFW_KEY_NUM_LOCK] = ZKey::NUM_LOCK;
    keyMap_[GLFW_KEY_PRINT_SCREEN] = ZKey::PRINT_SCREEN;
    keyMap_[GLFW_KEY_PAUSE] = ZKey::PAUSE;
    keyMap_[GLFW_KEY_F1] = ZKey::F1;
    keyMap_[GLFW_KEY_F2] = ZKey::F2;
    keyMap_[GLFW_KEY_F3] = ZKey::F3;
    keyMap_[GLFW_KEY_F4] = ZKey::F4;
    keyMap_[GLFW_KEY_F5] = ZKey::F5;
    keyMap_[GLFW_KEY_F6] = ZKey::F6;
    keyMap_[GLFW_KEY_F7] = ZKey::F7;
    keyMap_[GLFW_KEY_F8] = ZKey::F8;
    keyMap_[GLFW_KEY_F9] = ZKey::F9;
    keyMap_[GLFW_KEY_F10] = ZKey::F10;
    keyMap_[GLFW_KEY_F11] = ZKey::F11;
    keyMap_[GLFW_KEY_F12] = ZKey::F12;
    keyMap_[GLFW_KEY_F13] = ZKey::F13;
    keyMap_[GLFW_KEY_F14] = ZKey::F14;
    keyMap_[GLFW_KEY_F15] = ZKey::F15;
    keyMap_[GLFW_KEY_F16] = ZKey::F16;
    keyMap_[GLFW_KEY_F17] = ZKey::F17;
    keyMap_[GLFW_KEY_F18] = ZKey::F18;
    keyMap_[GLFW_KEY_F19] = ZKey::F19;
    keyMap_[GLFW_KEY_F20] = ZKey::F20;
    keyMap_[GLFW_KEY_F21] = ZKey::F21;
    keyMap_[GLFW_KEY_F22] = ZKey::F22;
    keyMap_[GLFW_KEY_F23] = ZKey::F23;
    keyMap_[GLFW_KEY_F24] = ZKey::F24;
    keyMap_[GLFW_KEY_F25] = ZKey::F25;
    keyMap_[GLFW_KEY_KP_0] = ZKey::KP_0;
    keyMap_[GLFW_KEY_KP_1] = ZKey::KP_1;
    keyMap_[GLFW_KEY_KP_2] = ZKey::KP_2;
    keyMap_[GLFW_KEY_KP_3] = ZKey::KP_3;
    keyMap_[GLFW_KEY_KP_4] = ZKey::KP_4;
    keyMap_[GLFW_KEY_KP_5] = ZKey::KP_5;
    keyMap_[GLFW_KEY_KP_6] = ZKey::KP_6;
    keyMap_[GLFW_KEY_KP_7] = ZKey::KP_7;
    keyMap_[GLFW_KEY_KP_8] = ZKey::KP_8;
    keyMap_[GLFW_KEY_KP_9] = ZKey::KP_9;
    keyMap_[GLFW_KEY_KP_DECIMAL] = ZKey::KP_DECIMAL;
    keyMap_[GLFW_KEY_KP_DIVIDE] = ZKey::KP_DIVIDE;
    keyMap_[GLFW_KEY_KP_MULTIPLY] = ZKey::KP_MULTIPLY;
    keyMap_[GLFW_KEY_KP_SUBTRACT] = ZKey::KP_SUBTRACT;
    keyMap_[GLFW_KEY_KP_ADD] = ZKey::KP_ADD;
    keyMap_[GLFW_KEY_KP_ENTER] = ZKey::KP_ENTER;
    keyMap_[GLFW_KEY_KP_EQUAL] = ZKey::KP_EQUAL;
    keyMap_[GLFW_KEY_LEFT_SHIFT] = ZKey::LEFT_SHIFT;
    keyMap_[GLFW_KEY_LEFT_CONTROL] = ZKey::LEFT_CONTROL;
    keyMap_[GLFW_KEY_LEFT_ALT] = ZKey::LEFT_ALT;
    keyMap_[GLFW_KEY_LEFT_SUPER] = ZKey::LEFT_SUPER;
    keyMap_[GLFW_KEY_RIGHT_SHIFT] = ZKey::RIGHT_SHIFT;
    keyMap_[GLFW_KEY_RIGHT_CONTROL] = ZKey::RIGHT_CONTROL;
    keyMap_[GLFW_KEY_RIGHT_ALT] = ZKey::RIGHT_ALT;
    keyMap_[GLFW_KEY_RIGHT_SUPER] = ZKey::RIGHT_SUPER;
    keyMap_[GLFW_KEY_MENU] = ZKey::MENU;

    mouseMap_[GLFW_MOUSE_BUTTON_LEFT] = ZMouse::LEFT_MB;
    mouseMap_[GLFW_MOUSE_BUTTON_RIGHT] = ZMouse::RIGHT_MB;
    mouseMap_[GLFW_MOUSE_BUTTON_MIDDLE] = ZMouse::MIDDLE_MB;
    mouseMap_[GLFW_MOUSE_BUTTON_4] = ZMouse::MB4;
    mouseMap_[GLFW_MOUSE_BUTTON_5] = ZMouse::MB5;
    mouseMap_[GLFW_MOUSE_BUTTON_6] = ZMouse::MB6;
    mouseMap_[GLFW_MOUSE_BUTTON_7] = ZMouse::MB7;
    mouseMap_[GLFW_MOUSE_BUTTON_8] = ZMouse::MB8;

    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwSetKeyCallback(windowHandle, &ZGLInput::KeyCallback);
    glfwSetMouseButtonCallback(windowHandle, &ZGLInput::MouseButtonCallback);
}

void ZGLInput::Update()
{
    GLFWwindow* windowHandle = glfwGetCurrentContext();

    if (keyPress_[ZKey::W])
    {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, 1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKey::A])
    {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(-1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKey::S])
    {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, -1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKey::D])
    {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[ZKey::ESCAPE])
    {
        std::shared_ptr<ZQuitEvent> quitEvent(new ZQuitEvent);
        zenith::EventAgent()->TriggerEvent(quitEvent);
    }

    double yaw, pitch;
    glfwGetCursorPos(windowHandle, &yaw, &pitch);

    if (firstLook_)
    {
        lastYaw_ = yaw; lastPitch_ = pitch;
        firstLook_ = false;
    }

    double deltaYaw = yaw - lastYaw_;
    double deltaPitch = lastPitch_ - pitch;

    if (mousePress_[ZMouse::LEFT_MB])
    {
        if (deltaYaw != 0.0 || deltaPitch != 0.0)
        {
            std::shared_ptr<ZObjectDragEvent> dragEvent(new ZObjectDragEvent(deltaYaw, deltaPitch, 0.f));
            zenith::EventAgent()->TriggerEvent(dragEvent);
        }
        else
        {
            std::shared_ptr<ZFireEvent> fireEvent(new ZFireEvent(yaw, pitch, 0.f));
            zenith::EventAgent()->TriggerEvent(fireEvent);
        }
    }

    if (deltaYaw != 0.0 || deltaPitch != 0.0)
    {
        std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(deltaYaw, deltaPitch));
        zenith::EventAgent()->TriggerEvent(lookEvent);
    }

    lastYaw_ = yaw; lastPitch_ = pitch;
}

void ZGLInput::GetCursorPosition(double& x, double& y)
{
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwGetCursorPos(windowHandle, &x, &y);
}

void ZGLInput::SetCursorPosition(double& x, double& y)
{
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwSetCursorPos(windowHandle, x, y);
}

void ZGLInput::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    zenith::Input()->SetKey(keyMap_[key], action != GLFW_RELEASE);
}

void ZGLInput::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    zenith::Input()->SetMouse(mouseMap_[button], action == GLFW_PRESS);
}
