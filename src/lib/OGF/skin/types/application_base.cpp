/*
 *  GXML/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000 Bruno Levy
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#include <OGF/skin/types/application_base.h>
#include <OGF/skin/types/preferences.h>
#include <OGF/gom/interpreter/interpreter.h>
#include <OGF/gom/reflection/meta.h>
#include <OGF/basic/modules/modmgr.h>
#include <OGF/basic/os/file_manager.h>

#include <geogram/basic/environment.h>
#include <geogram/basic/process.h>
#include <geogram/basic/file_system.h>
#include <geogram/basic/line_stream.h>
#include <geogram/basic/command_line.h>
// #include <geogram/third_party/gzstream/gzstream.h>

#include <vector>
#include <fstream>
#include <algorithm>

namespace OGF {

    bool ApplicationBase::stopping_ = true;
    
    ApplicationBase::ApplicationBase(Interpreter* interpreter) :
	interpreter_(interpreter)
    {
	logger_client_ = new ApplicationBaseLoggerClient(this);
	progress_client_ = new ApplicationBaseProgressClient(this);
	Logger::instance()->register_client(logger_client_);
	Progress::set_client(progress_client_);
    }

    ApplicationBase::~ApplicationBase() {
	if(logger_client_ != nullptr) {
	    Logger::instance()->unregister_client(logger_client_);
	}
	Progress::set_client(nullptr);
    }

    void ApplicationBase::start() {
	stopping_ = false;
        // User GEL files
        if(Environment::instance()->has_value("gel:startup_files")) {
            std::string gel_str = 
                Environment::instance()->get_value("gel:startup_files");
            std::vector<std::string> gel_files;
            String::split_string(gel_str, ';', gel_files);
            for(unsigned int i=0; i<gel_files.size(); i++) {
                Logger::out("GEL")
                    << "Loading file: " << gel_files[i] << std::endl;
                interpreter()->execute_file(gel_files[i]);
            }
        }
        started();
    }
    
    void ApplicationBase::stop() {
        Logger::instance()->unregister_client(logger_client_);
	logger_client_ = nullptr;
        Progress::set_client(nullptr);
    }
    
    void ApplicationBase::set_style(const std::string& value) {
        Environment::instance()->set_value("gfx:style", value);
    }

    void ApplicationBase::set_font_size(index_t value) {
        std::string value_string;
        ogf_convert_to_string(value, value_string);
        Environment::instance()->set_value("gui:font_size", value_string);
    }

    void ApplicationBase::declare_preference_variable(
	const std::string& name,
	const std::string& value,
	const std::string& help
    ) {
	if(CmdLine::arg_is_declared(name)) {
	    Preferences::declare_preference_variable(name);
	} else {
	    Preferences::declare_preference_variable(
		name, value.c_str(), help
	    );
	}
    }
    
    void ApplicationBase::save_preferences() {
        Preferences::save_preferences();
    }

    void ApplicationBase::load_preferences(const std::string& filename) {
	CmdLine::load_config(filename, CmdLine::argv()[0]);
    }

    bool ApplicationBase::preferences_loaded() {
	return CmdLine::config_file_loaded();
    }
    
    void ApplicationBase::progress(index_t step, index_t percent) {
        geo_argused(step);
        notify_progress(percent);
	draw();
    }
    
    void ApplicationBase::cancel_current_job() {
        Logger::out("Task") << "Canceled current job" << std::endl;
        Progress::cancel();
    }

    void ApplicationBase::begin() {
        const std::string& task_name =
            Progress::current_progress_task()->task_name();
        Logger::out(task_name) << "Running..." << std::endl;
        notify_progress_begin(task_name);
    }

    void ApplicationBase::end(bool canceled) {
        const std::string& task_name =
            Progress::current_progress_task()->task_name();
        if(canceled) {
            Logger::out(task_name) << "interrupted." << std::endl;
            notify_progress(0);
        } 
        notify_progress_end();
    }

    std::string ApplicationBase::find_file(const std::string& filename) const {
	std::string result = filename;
	if(!FileManager::instance()->find_file(result)) {
	    result = "";
	}
	return result;
    }
    
    void ApplicationBase::draw() {
    }

    void ApplicationBase::update() {
    }

/**************************************************************/
    
    ApplicationBase::
    ApplicationBaseProgressClient::ApplicationBaseProgressClient(
        ApplicationBase* app
    ) : application_base_(app) {
    }

    void ApplicationBase::
    ApplicationBaseProgressClient::begin() {
        application_base_->begin();
    }

    void ApplicationBase::
    ApplicationBaseProgressClient::progress(index_t step, index_t percent) {
        application_base_->progress(step, percent);
    }

    void ApplicationBase::
    ApplicationBaseProgressClient::end(bool canceled) {
        application_base_->end(canceled);
    }
    
    
/**************************************************************/    
    
    ApplicationBase::
    ApplicationBaseLoggerClient::ApplicationBaseLoggerClient(
        ApplicationBase* app
    ) : application_base_(app) {
    }

    void ApplicationBase::    
    ApplicationBaseLoggerClient::div(const std::string& value) {
        application_base_->div(value);
    }

    void ApplicationBase::        
    ApplicationBaseLoggerClient::out(const std::string& value) {
        application_base_->out(value);        
    }

    void ApplicationBase::        
    ApplicationBaseLoggerClient::warn(const std::string& value) {
        application_base_->warn(value);        
    }

    void ApplicationBase::        
    ApplicationBaseLoggerClient::err(const std::string& value) {
        application_base_->err(value);        
    }

    void ApplicationBase::        
    ApplicationBaseLoggerClient::status(const std::string& value) {
        application_base_->status(value);        
    }
    
/**************************************************************/        
    
}
