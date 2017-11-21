uenv
====

`uenv` executes a command with environment variables loaded from a file.

- [Usage](#usage)
- [Examples](#examples)
- [Installation](#installation)
- [Limitations](#limitations)
- [Similar utilities](#similar-utilities)
- [License](#license)

## Usage

    Usage: uenv [-ch] [-f envfile] command ...
    
    Options
      -c          clear environment variables first
      -f envfile  load environment variables from envfile (default: .env)
      -h          show this help

An envfile is a text file containing `key=value` pairs delimited by newlines,
possibly with comment lines starting with `#`. `uenv` loads the envfile, sets
environment variables accordingly, and executes the command.

The exit code is 111 if any error occured before executing the command.
Otherwise the exit code is the same as that of the command.

## Examples

### Basic usage

Load environment variables from `.env` file and run `vagrant up`:

    $ cat .env
    API_KEY=abcdefghijklmnopeqrstvwxyz
    REGION=Tokyo
    $ uenv vagrant up

### Envfile as a script

    $ cat test.env
    #!/usr/local/bin/uenv -cf
    USERNAME=johndoe
    PASSWORD=correcthorsebatterystaple
    $ ./test.env env
    PATH=/usr/bin:/bin:/usr/sbin:/sbin
    USERNAME=johndoe
    PASSWORD=correcthorsebatterystaple

## Installation

Requirements:

- C99
- POSIX

Build and install:

    make
    cp uenv ~/bin

## Limitations

- Environment variables may not contain newlines
- The length of each line in a envfile must not exceed 32768 bytes (including
  the newline character)

## Similar utilities

- [envdir](https://cr.yp.to/daemontools/envdir.html)
- [dotenv](https://github.com/bkeepers/dotenv)
- [foreman](https://github.com/ddollar/foreman)
- [direnv](https://github.com/direnv/direnv)

## License

MIT
