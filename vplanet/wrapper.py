# -*- coding: utf-8 -*-
from . import vplanet_core as core
from .output import get_output
import sys
import subprocess
import os
import re


class VPLANETError(RuntimeError):
    """
    Catch-all runtime error class for VPLANET.

    """

    pass


class VPLANETHelp:
    """
    VPLANET help message wrapper.
    
    """

    def __init__(self):
        self._help = subprocess.check_output(["vplanet", "-h"]).decode("utf-8")

    def __repr__(self):
        return self._help

    def __str__(self):
        return self._help


def _entry_point():
    """
    ``vplanet`` command line script entry point.

    """
    return core.run(*sys.argv)


def run(infile="vpl.in", verbose=False, quiet=False, clobber=False, units=True):
    """
    Run `vplanet` and return the output.

    Args:
        infile (str, optional): The path to the input file. Default ``vpl.in``.
        verbose (bool, optional): Enable verbose output? Default False.
        quiet (bool, optional): Suppress all output? Default False.
        clobber (bool, optional): Run ``vplanet`` even if a log file exists?
            Default False.
        units (bool, optional): If True, returns unit-ful output. If False, the
            output arrays are standard ``numpy`` arrays. Default True.

    Returns:
        A ``vplanet.Output`` object containing the full output from the run.

    Raises:
        ``vplanet.VPLANETError``: If something goes wrong in the C extension.

    .. note:: 
    
        We need to change the way vplanet exits on error in order to
        return to the Python interpreter when it terminates. The current
        hack is to spawn a subprocess so we don't terminate the current 
        Python session.

    """
    # Determine the system name from the infile
    sysname = None
    with open(infile, "r") as f:
        lines = f.readlines()
        for line in lines:
            match = re.match("sSystemName[ \t\n]+(.*?)[ \t\n#]", line)
            if match:
                if len(match.groups()):
                    sysname = match.groups()[0]
                    break

    # Does the log file exist?
    path = os.path.abspath(os.path.dirname(infile))
    log_exists = os.path.exists(os.path.join(path, "{}.log".format(sysname)))

    # Run vplanet
    if clobber or not log_exists:

        # Parse kwargs
        args = ["vplanet", infile]
        if verbose:
            args += ["-v"]
        if quiet:
            args += ["-q"]

        # Spawn `vplanet` as a subprocess
        error = False
        try:
            subprocess.check_output(args, cwd=path)
        except subprocess.CalledProcessError as e:
            error = True
        if error:
            raise VPLANETError("Error running VPLANET.")

    # Grab the output
    output = get_output(path=path, sysname=sysname, units=units)

    # We're done!
    return output


def help():
    """
    Display the VPLANET help message.

    """
    return VPLANETHelp()
