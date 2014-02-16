#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include <lua5.1/lua.hpp>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

class CConfigReader
{
	private:
		lua_State *m_state;

		void type_error() const
		{
			const char *error = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);
			throw std::logic_error(error);
		}

		void get_param(const char *name)
		{
			lua_pushstring(m_state, name);
			lua_getglobal(m_state, "get_param_value");
			lua_getglobal(m_state, "configs");
			lua_pushstring(m_state, name);
			if (lua_pcall(m_state, 2, 1, 0))
			{
				const char *error = lua_tostring(m_state, -1);
				lua_pop(m_state, 1);
				throw std::runtime_error(error);
			}
		}

	public:
		CConfigReader():
			m_state(lua_open())
		{
			luaL_openlibs(m_state);
			if (luaL_dofile(m_state, "config-reader.lua"))
			{
				const char *error = lua_tostring(m_state, -1);
				lua_pop(m_state, 1);
				throw std::runtime_error(error);
			}
		}

		~CConfigReader()
		{
			if (NULL != m_state)
			{
				lua_close(m_state);
			}
		}

		bool readbool(const char *name)
		{
			get_param(name);
			if (!lua_isboolean(m_state, -1))
			{
				type_error();
			}
			bool result = (bool) lua_toboolean(m_state, -1);
			lua_pop(m_state, 1);
			return result;
		}

		double readnumber(const char *name)
		{
			get_param(name);
			if (!lua_isnumber(m_state, -1))
			{
				type_error();
			}
			double result = lua_tonumber(m_state, -1);
			lua_pop(m_state, 1);
			return result;
		}

		std::string readstring(const char *name)
		{
			get_param(name);
			if (!lua_isstring(m_state, -1))
			{
				type_error();
			}
			const char *result = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);
			return result;
		}
};

int main()
{
	try
	{
		/*
		 * otherfamily:
		 * 	1 (number): 'Rhu Barber' (string)
		 * 	2 (number): 'Harry Barber' (string)
		 * needspeeling (string): true (boolean)
		 * favouritefruit (string): 'banana' (string)
		 * fullname (string): 'Foo Barber' (string)
		 * age (string): 21 (number)
		 */
		CConfigReader config;
		std::cout << "Name "
			<< config.readstring("simple.fullname") << " ("
			<< config.readnumber("json.age") << " years old) should "
			<< (config.readbool("json.needspelling") ? "" : "not ")
			<< "be spelled" << std::endl;
	} catch (const std::exception &e)
	{
		std::cout << "An exception was occured while running the program: "
			<< e.what() << std::endl;
	}

	return 0;
}
