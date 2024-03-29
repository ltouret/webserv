#include "config_handler.hpp"

config_handler::config_handler(void): _ipAddress("127.0.0.1"), _port(8080), _serverNames(), _errorPages(), \
						_clientMaxBodySize(CLIENTMAXBODYSIZE), _cgiPass(), _allowMethods(), _location(), \
						_root(), _index(), _autoIndex(false), _uploadFolder(), _redirection() {}

config_handler::~config_handler(void) {}

config_handler::config_handler(config_handler const & other): _ipAddress(other._ipAddress), _port(other._port), \
										_serverNames(other._serverNames), _errorPages(other._errorPages), \
										_clientMaxBodySize(other._clientMaxBodySize), _cgiPass(other._cgiPass), \
										_allowMethods(other._allowMethods), _location(other._location), \
										_root(other._root), _index(other._index), _autoIndex(other._autoIndex), \
										_uploadFolder(other._uploadFolder), _redirection(other._redirection) {}

config_handler & config_handler::operator=(config_handler const & other)
{
	if (this != &other)
	{
		this->_ipAddress = other._ipAddress;
		this->_port = other._port;
		this->_serverNames = other._serverNames;
		this->_errorPages = other._errorPages;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_cgiPass = other._cgiPass;
		this->_allowMethods = other._allowMethods;
		this->_location = other._location;
		this->_root = other._root;
		this->_index = other._index;
		this->_autoIndex = other._autoIndex;
		this->_uploadFolder = other._uploadFolder;
		this->_redirection = other._redirection;
	}
	return (*this);
}

// GET

std::string							&config_handler::getIpAddress(void)
{
	return this->_ipAddress;
}

int									&config_handler::getPort(void)
{
	return this->_port;
}

std::vector<std::string>			&config_handler::getserverNames(void)
{
	return this->_serverNames;
}

std::map<int, std::string>			&config_handler::getErrorPages(void)
{
	return this->_errorPages;
}

unsigned long long					&config_handler::getclientMaxBodySize(void)
{
	return this->_clientMaxBodySize;
}

std::string							&config_handler::getcgiPass(void)
{
	return this->_cgiPass;
}

std::vector<std::string>			&config_handler::getAlowMethods(void)
{
	return this->_allowMethods;
}

std::map<std::string, config_handler>		&config_handler::getLocation(void)
{
	return this->_location;
}

std::string							&config_handler::getRoot(void)
{
	return this->_root;
}

std::vector<std::string>			&config_handler::getIndex(void)
{
	return this->_index;
}

bool								&config_handler::getAutoIndex(void)
{
	return this->_autoIndex;
}

std::string							&config_handler::getUploadFolder(void)
{
	return this->_uploadFolder;
}

std::pair<std::string, std::string>	&config_handler::getRedirection(void)
{
	return this->_redirection;
}

int	config_handler::parseserver(std::vector<std::vector<std::string> > confFile, size_t i)
{
	for (i++; i < confFile.size(); i++)
	{
		if (confFile[i][0].compare("server") == 0)
			throw std::runtime_error("Error: Bad server{} config\n");
		if (confFile[i][0].compare("}") == 0)
			return i;
		if (confFile[i][0].compare("listen") == 0)
			this->_setListen(confFile[i]);
		if (confFile[i][0].compare("server_name") == 0)
			this->_setserverName(confFile[i]);
		if (confFile[i][0].compare("error_page") == 0)
			this->_setErrorPage(confFile[i]);
		if (confFile[i][0].compare("client_max_body_size") == 0)
			this->_setclientMaxBodySize(confFile[i]);
		if (confFile[i][0].compare("cgi_pass") == 0)
			this->_setcgiPass(confFile[i]);
		if (confFile[i][0].compare("allow_methods") == 0)
			this->_setAllowMethods(confFile[i]);
		if (confFile[i][0].compare("location") == 0)
			i = this->_setLocation(confFile, i);
		if (confFile[i][0].compare("root") == 0)
			this->_setRoot(confFile[i]);
		if (confFile[i][0].compare("index") == 0)
			this->_setIndex(confFile[i]);
		if (confFile[i][0].compare("autoindex") == 0)
			this->_setAutoIndex(confFile[i]);
		if (confFile[i][0].compare("upload_store") == 0)
			this->_setUploadFolder(confFile[i]);
		if (confFile[i][0].compare("return") == 0)
			this->_setRedirection(confFile[i]);
	}
	throw std::runtime_error("Error: server{} not closed\n");
}

void config_handler::checkBlock(bool location)
{
	if (this->_ipAddress.compare("localhost") == 0)
		this->_ipAddress = "127.0.0.1";
	if (this->_serverNames.empty())
		this->_serverNames.push_back("");
	if (!location && this->_root.empty())
		this->_root = "./www";
	if (!this->_location.empty())
	{
		for (std::map<std::string, config_handler>::iterator it = _location.begin(); it != _location.end(); it++)
			it->second.checkBlock(true);
	}
}

// SET

void config_handler::_setListen(std::vector<std::string> line)
{
	size_t	cut;

	if (line.size() == 1)
		throw std::runtime_error("Error: Bad listen config\n");
	if ((cut = line[1].find(":")) == std::string::npos)
	{
		if (line[1].compare("localhost") == 0 || line[1].find(".") != std::string::npos)
			this->_ipAddress = line[1];
		else
		{
			for (std::string::iterator it = line[1].begin(); it != line[1].end(); it++)
				if (isdigit(*it) == 0)
					throw std::runtime_error("Error: Bad listen config\n");
			this->_port = atoi(line[1].c_str());
		}
	}
	else
	{
		this->_ipAddress = line[1].substr(0, cut);
		if (isdigit(atoi(line[1].substr(cut).c_str())) == 0)
			this->_port = atoi(line[1].substr(cut + 1).c_str());
		else
			throw std::runtime_error("Error: Bad listen config\n");
	}
}

void config_handler::_setserverName(std::vector<std::string> line)
{
	if (line.size() == 1)
		throw std::runtime_error("Error: Bad server_name config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_serverNames.push_back(line[i]);
}

void config_handler::_setErrorPage(std::vector<std::string> line)
{
	if (line.size() < 3)
		throw std::runtime_error("Error: Bad error_page config\n");

	std::string uri = line[line.size() - 1];
	if (uri[0] == '/')
		uri.erase(0, 1);
	if (pathIsFile(uri) != 1)
		throw std::runtime_error("Error: Bad error_page path\n");
	for (size_t i = 1; i < line.size() - 1; i++)
		this->_errorPages.insert(std::pair<int, std::string>(atoi(line[i].c_str()), uri));
}

void config_handler::_setclientMaxBodySize(std::vector<std::string> line)
{
	size_t	pos;

	if (line.size() != 2)
		throw std::runtime_error("Error: Bad client_max_body_size config\n");
	pos = line[1].find_first_not_of("0123456789");
	if (pos == 0)
		throw std::runtime_error("Error: Bad client_max_body_size config\n");
	this->_clientMaxBodySize = atoi(line[1].c_str());
	if (line[1][pos] == 'K' || line[1][pos] == 'k')
		this->_clientMaxBodySize *= 1000;
	if (line[1][pos] == 'M' || line[1][pos] == 'm')
		this->_clientMaxBodySize *= 1000000;
	if (line[1][pos] == 'G' || line[1][pos] == 'g')
		this->_clientMaxBodySize *= 1000000000;
}

void config_handler::_setcgiPass(std::vector<std::string> line)
{
	if (line.size() != 2)
		throw std::runtime_error("Error: Bad cgi_pass config\n");
	this->_cgiPass = line[1].c_str();
}

void config_handler::_setAllowMethods(std::vector<std::string> line)
{
	if (line.size() == 1)
		throw std::runtime_error("Error: Bad allow_methods config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_allowMethods.push_back(line[i]);
}

int config_handler::_setLocation(std::vector<std::vector<std::string> > confFile, size_t i)
{
	config_handler		location;

	if (confFile[i].size() == 3)
	{
		std::string	path = confFile[i][1];
		this->_removeLastSlashe(path);
		if (path[0] != '/')
			path.insert(0, 1, '/');
		if (confFile[i][0].compare("location") == 0 && confFile[i][2].compare("{") == 0)
		{
			i = location._parseLocationDeep(confFile, i);
			this->_location[path] = location;
		}
		else
			throw std::runtime_error("Error: Bad location configuration\n");
	}
	else
		throw std::runtime_error("Error: Bad location configuration\n");
	return i;
}

int	config_handler::_parseLocationDeep(std::vector<std::vector<std::string> > confFile, size_t i)
{
	for (i++; i < confFile.size(); i++)
	{
		if (confFile[i][0].compare("}") == 0)
			return i;
		if (confFile[i][0].compare("listen") == 0 || confFile[i][0].compare("server_name") == 0)
			throw std::runtime_error("Error: Bad location config\n");
		if (confFile[i][0].compare("error_page") == 0)
			this->_setErrorPage(confFile[i]);
		if (confFile[i][0].compare("client_max_body_size") == 0)
			this->_setclientMaxBodySize(confFile[i]);
		if (confFile[i][0].compare("cgi_pass") == 0)
			this->_setcgiPass(confFile[i]);
		if (confFile[i][0].compare("allow_methods") == 0)
			this->_setAllowMethods(confFile[i]);
		if (confFile[i][0].compare("location") == 0)
			i = this->_setLocation(confFile, i);
		if (confFile[i][0].compare("root") == 0)
			this->_setRoot(confFile[i]);
		if (confFile[i][0].compare("index") == 0)
			this->_setIndex(confFile[i]);
		if (confFile[i][0].compare("autoindex") == 0)
			this->_setAutoIndex(confFile[i]);
		if (confFile[i][0].compare("upload_store") == 0)
			this->_setUploadFolder(confFile[i]);
		if (confFile[i][0].compare("return") == 0)
			this->_setRedirection(confFile[i]);
	}
	throw std::runtime_error("Error: location{} not closed\n");
}

void config_handler::_setRoot(std::vector<std::string> line)
{
	if (line.size() != 2)
		throw std::runtime_error("Error: Bad root config\n");
	this->_root = line[1];
	this->_removeLastSlashe(this->_root);
}

void config_handler::_setIndex(std::vector<std::string> line)
{
	if (line.size() == 1)
		throw std::runtime_error("Error: Bad index config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_index.push_back(line[i]);
}

void config_handler::_setAutoIndex(std::vector<std::string> line)
{
	if (line.size() != 2)
		throw std::runtime_error("Error: Bad autoindex config\n");
	if (line[1].compare("on") == 0)
		this->_autoIndex = true;
}

void config_handler::_setUploadFolder(std::vector<std::string> line)
{

	if (line.size() != 2)
		throw std::runtime_error("Error: Bad upload config\n");
	this->_uploadFolder = line[1].c_str();
}

void config_handler::_setRedirection(std::vector<std::string> line)
{
	if (line.size() < 2 || line.size() > 3 || (line[1].compare("301") == 0 && line.size() != 3))
		throw std::runtime_error("Error: Bad return config\n");
	for (std::string::iterator it = line[1].begin(); it != line[1].end(); it++)
		if (isdigit(*it) == 0)
			throw std::runtime_error("Error: Bad return config\n");
	this->_redirection.first = line[1];
	if (line.size() == 3)
		this->_redirection.second = line[2];
}

void	config_handler::_removeLastSlashe(std::string &path)
{
	if (path.find_last_of('/') == path.size() - 1 && path.size() != 1)
		path.erase(path.size() - 1, 1);
}

std::ostream	&operator<<(std::ostream &out, config_handler &conf)
{
	out << "IP = " << conf.getIpAddress() << std::endl;
	out << "Port = " << conf.getPort() << std::endl;
	out << "serverNames = ";
	for (size_t i = 0; i < conf.getserverNames().size(); i++)
	{
		out << conf.getserverNames()[i];
		if (i != conf.getserverNames().size() - 1)
			out << " ";
	}
	out << std::endl;
	for (std::map<int, std::string>::iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); it++)
		out << "Error = " << it->first << ": File = " << it->second << std::endl;
	out << "clientMaxBodySize = " << conf.getclientMaxBodySize() << std::endl;
	out << "cgiPass = " << conf.getcgiPass() << std::endl;
	out << "AlowMethods = ";
	for (size_t i = 0; i < conf.getAlowMethods().size(); i++)
	{
		out << conf.getAlowMethods()[i];
		if (i != conf.getAlowMethods().size() - 1)
			out << " ";
	}
	out << std::endl;
	for (std::map<std::string, config_handler>::iterator it = conf.getLocation().begin(); it != conf.getLocation().end(); it++)
		out << "Location " << it->first << " [\n" << it->second << "]" << std::endl;
	out << "Root = " << conf.getRoot() << std::endl;
	out << "Index = ";
	for (size_t i = 0; i < conf.getIndex().size(); i++)
	{
		out << conf.getIndex()[i];
		if (i != conf.getIndex().size() - 1)
			out << " ";
	}
	out << std::endl;
	out << "AutoIndex = " << (conf.getAutoIndex() ? "true" : "false") << std::endl;
	out << "Upload_folder = " << conf.getUploadFolder() << std::endl;
	out << "Return = " << conf.getRedirection().first;
	if (!conf.getRedirection().second.empty())
		out << " : Location = " << conf.getRedirection().second;
	out << std::endl;
	return out;
}
