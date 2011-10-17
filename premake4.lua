solution "LEAN"
	configurations { "debug", "release" }

		local all_files =
		{
			"gKit/*.cpp", "gKit/*.h",
			"gKit/GL/*.cpp", "gKit/GL/*.h",

			"gKit/Widgets/nvwidgets/nvWidgets.cpp", "gKit/Widgets/nvwidgets/nvGLWidgets.cpp",
			"gKit/Widgets/nvwidgets/nvSdlWidgets.cpp", "gKit/Widgets/nvwidgets/nvSdlTextPainter.cpp",
			"gKit/Widgets/nvglutils/*.h",
		}

		includedirs
		{
			".",
			"gKit",
			"gKit/Widgets"
		}

		configuration "debug"
			defines { "GK_OPENGL3", "DEBUG", "VERBOSE" }
			flags { "Symbols" }

		configuration "release"
			defines { "GK_OPENGL3", "NDEBUG", "VERBOSE" }
			flags { "Optimize" }

		configuration "linux"
			includedirs { "glew-1.7.0/include" }
			buildoptions { "-pipe", "`sdl-config --cflags`", "-march=native" }
			linkoptions { "-Wl,-rpath,glew-1.7.0/lib -Lglew-1.7.0/lib -lGLEW" } -- forcer la version locale de GLEW
			linkoptions { "`sdl-config --libs`" }
			links { "GL", "SDL_image", "SDL_ttf" }

		configuration { "linux", "release" }
			buildoptions { "-march=native -mfpmath=sse -msse3" }
--~ 		-funsafe-math-optimizations -ffinite-math-only -fno-signed-zeros -fno-trapping-math -mrecip -ftree-vectorize

		configuration "windows"
			includedirs { "./glew-1.7.0/include", "./SDL-1.2.14/include", "./SDL_image-1.2.10/include", "./SDL_ttf-2.0.10/include" } -- configurer les libs dans visual
			libdirs { "./glew-1.7.0/lib", "./SDL-1.2.14/lib", "./SDL_image-1.2.10/lib", "./SDL_ttf-2.0.10/lib" } -- configurer les libs dans visual
			defines { "WIN32", "NVWIDGETS_EXPORTS", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
			defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((
			defines { "NVWIDGETS_EXPORTS" } -- for gKitStatic lib
			links { "opengl32", "glu32", "glew32", "SDL", "SDLmain", "SDL_image", "SDL_ttf" }
			linkoptions { "/NODEFAULTLIB:msvcrt.lib" }

		configuration "macosx"
			defines { "APPLE_OSX", "__APPLE_CC__"}
			files
			{
				"SDLMain.m"
			}
			buildoptions {"-framework OpenGL -framework SDL -framework SDL_image -framework SDL_ttf -framework Cocoa -I$HOME/local/include"}
			linkoptions {"-framework OpenGL -framework SDL -framework SDL_image -framework SDL_ttf -framework Cocoa -L$HOME/local/lib"}
			links {"GLEW", "RGBE"}


project "gKitStatic"
 	language "C++"
 	kind "StaticLib"
 	files(all_files)

project "gKitShared"
 	language "C++"
 	kind "SharedLib"
 	files(all_files)
 	buildoptions {"-fPIC"}

local all_main_files = {
	"image_viewer"
}

for i, name in ipairs(all_main_files) do
	project(name)
		language "C++"
		kind "ConsoleApp"
 		links {"gKitStatic"}
--		links {"gKitShared"}
		files {name..'.cpp'}
end
