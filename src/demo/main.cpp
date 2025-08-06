#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "renderer/RendererGL.h"
#include "vector/Context.h"
#include "geometry/Circle.h"
#include "geometry/RoundedRect.h"
#include "geometry/Triangle.h"
#include "geometry/Rect.h"
#include "geometry/Line.h"
#include "renderer/MsdfTextRenderer.h"
#include "renderer/Paint.h"

void drawFlatButton(Context& vg, float x, float y, float w, float h, float r, const std::string& label) {
	glm::vec4 bgColor = { 0.17f, 0.24f, 0.45f, 1.0f }; // deep blue
	glm::vec4 outlineCol = { 0.27f, 0.36f, 0.54f, 1.0f };
	glm::vec4 textColor = { 0.98f, 0.98f, 1.0f, 1 };

	vg.setFillColor(bgColor);
	vg.fillRoundedRect(RoundedRect{ x, y, w, h, r});

    vg.setOutlineColor(outlineCol);
    vg.setOutlineWidth(1.2f);
    vg.setFillColor(glm::vec4{ x + 0.5f, y + 0.5f, w - 1, h - 1 });

    vg.setFillColor(textColor);
    //vg.drawText(glm::vec2(x + w / 2), glm::vec2(y + h / 2), label);
}

void draw3DButton(Context& vg, float x, float y, float w, float h, float r, const std::string& label) {
    // Colors (tweak to your liking)
    glm::vec4 shadowColor      = {0.0f, 0.0f, 0.0f, 0.30f};            // strong shadow
    glm::vec4 lightColor       = {0.22f, 0.36f, 0.64f, 1.0f};           // top gradient: lighter blue
    glm::vec4 darkColor        = {0.08f, 0.15f, 0.32f, 1.0f};           // bottom gradient: very dark blue
    glm::vec4 outlineColor     = {0.12f, 0.22f, 0.38f, 1.0f};           // slightly lighter outline
    glm::vec4 glossColor       = {0.88f, 0.98f, 1.0f, 0.15f};           // subtle blueish-white gloss
    glm::vec4 textColor        = {0.91f, 0.95f, 1.0f, 1.0f};            // bright off-white
    glm::vec4 textShadowColor  = {0.0f, 0.0f, 0.0f, 0.35f};             // strong text shadow

    float shadowOffset = 2.5f;
    float outlineWidth = 1.2f;
    float glossPad     = 4.0f;

    // 1. Drop shadow (drawn first, under everything)
    vg.setFillColor(shadowColor);
	vg.fillRoundedRect(RoundedRect{ x, y + shadowOffset, w, h, r });

    // 2. Button background gradient
    vg.setLinearGradient(glm::vec2(x, y), glm::vec2(x, y + h), lightColor, darkColor);
	vg.fillRoundedRect(RoundedRect{ x, y, w, h, r });

    // 3. Glossy top highlight (top 40% only)
    //Paint gloss = Paint::Linear(
    //    { x, y }, { x, y + h * 0.4f },
    //    glossColor, glm::vec4(glossColor.r, glossColor.g, glossColor.b, 0.0f)
    //);
    vg.setLinearGradient(glm::vec2{ x, y }, glm::vec2{ x, y + h * 0.4f },
        glossColor, glm::vec4(glossColor.r, glossColor.g, glossColor.b, 0.0f));

    vg.fillRoundedRect(RoundedRect{ x + glossPad, y + glossPad, w - 2 * glossPad, h * 0.4f , r - glossPad });

    // 4. Button outline
    vg.setOutlineColor(outlineColor);
    vg.setOutlineWidth(outlineWidth);
    vg.setFillColor(glm::vec4{ x + 0.5f, y + 0.5f, w - 1, h - 1 });

    // 5. Button label shadow (slightly offset)
    //vg.setFillColor(textShadowColor);
    //vg.drawText(x + w / 2 + 1, y + h / 2 + 2, label);

    //// 6. Button label main text
    //vg.setFillColor(textColor);
    //vg.drawText(x + w / 2, y + h / 2, label);
}

int main() {
	if (!glfwInit()) return -1;
	GLFWwindow* window = glfwCreateWindow(900, 500, "SDF Vector Engine Backend Demo", nullptr, nullptr);
	if (!window) return -1;
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ShaderManager shaderMgr;
	RendererGL backend(900, 500, shaderMgr);
	Context vg(backend);

	// --- Geometry: just pure data ---
	Circle        circle{ 110, 100, 60 };
	RoundedRect   roundedRect{ 240, 50, 120, 100, 30 };
	Triangle      triangle{ glm::vec2(420, 160), glm::vec2(480, 60), glm::vec2(540, 170) };
	Rect          rect{ 580, 60, 120, 90 };
	Line          line{ glm::vec2(740, 60), glm::vec2(860, 170) };
	Circle        circleG{ 110, 370, 60 };
	RoundedRect   roundedRectG{ 240, 320, 120, 100, 30 };
	Triangle      triangleG{ glm::vec2(420, 430), glm::vec2(480, 330), glm::vec2(540, 440) };
	Rect          rectG{ 580, 320, 120, 90 };
	Line          lineG{ glm::vec2(740, 330), glm::vec2(860, 440) };

	// --- Set up MSDF text renderer ---
	MsdfTextRenderer textRenderer;
	if (!textRenderer.load("assets/font-msdf.png", "assets/font-msdf.json", 4.0f)) {
		printf("Failed to load MSDF font atlas or metadata!\n");
		return 1;
	}

	// --- Projection ---
	glm::mat4 proj = glm::ortho(0.0f, float(900), float(500), 0.0f);

	while (!glfwWindowShouldClose(window)) {
		vg.beginFrame();

		//draw3DButton(vg, 200, 200, 100, 30, 10, "OK");
		//drawFlatButton(vg, 200, 200, 100, 30, 10, "OK");

        vg.setOutlineColor({ 1,0,0,1 });   // Black outline
        vg.setOutlineWidth(4.0f);        // 3 pixels wide

        // --- Solid color row (top) ---
        vg.setFillColor({ 0.0f, 0.65f, 1.0f, 1.0f });
        vg.fillCircle(circle);

        vg.setFillColor({ 1.0f, 0.4f, 0.3f, 1.0f });
        vg.fillRoundedRect(roundedRect);

        vg.setFillColor({ 1.0f, 0.8f, 0.15f, 1.0f });
        vg.fillTriangle(triangle);

        vg.setFillColor({ 0.3f, 1.0f, 0.4f, 1.0f });
        vg.fillRect(rect);

        vg.setFillColor({ 1.0f, 0.5f, 1.0f, 1.0f });
        vg.drawLine(line, 8.0f);

		// --- Draw "Hello World" text using MSDF renderer ---
		glm::vec4 textColor = { 1.0f, 0.98f, 0.95f, 1.0f };
		float textX = 50, textY = 250;
		float pxSize = 32.0f; // matches your atlas
		float scale = pxSize;   // 1.0 for pxSize = 48, or adjust

        textRenderer.drawText("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", textX, textY, scale, textColor, proj);

        textRenderer.drawText("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", textX, 280, 24, textColor, proj);

        textRenderer.drawText("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", textX, 300, 18, textColor, proj);

		vg.setOutlineWidth(0.0f);

		// --- Gradient row (bottom) ---
        vg.setLinearGradient({ 60,320 }, { 160,420 }, { 0.0f,0.65f,1.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f });
        vg.fillCircle(circleG);

        vg.setLinearGradient({ 240,370 }, { 360,470 }, { 1.0f,0.4f,0.3f,1.0f }, { 0.3f,1.0f,0.4f,1.0f });
        vg.fillRoundedRect(roundedRectG);

        vg.setLinearGradient({ 420,320 }, { 540,370 }, { 1.0f,0.8f,0.15f,1.0f }, { 0.0f,0.65f,1.0f,1.0f });
        vg.fillTriangle(triangleG);

        vg.setLinearGradient({ 580,320 }, { 700,410 }, { 0.3f,1.0f,0.4f,1.0f }, { 1.0f,0.5f,1.0f,1.0f });
        vg.fillRect(rectG);

        vg.setLinearGradient({ 740,320 }, { 860,370 }, { 1.0f,0.5f,1.0f,1.0f }, { 0.3f,1.0f,0.4f,1.0f });
        vg.drawLine(lineG, 8.0f);

		vg.endFrame();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
