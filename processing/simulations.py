import os
import subprocess
import ConfigParser

USER = 'eviatarbach'
TESTOUTPUT = '../../../testoutput/' + USER
DEFAULTS = {'cells_across': 20,
            'cells_up': 10,
            'division_volume': 1.7,
            'end_time': 10.0,
            'sampling_rate': 50,
            'adhesion01': 10.0,
            'adhesion11': 2.0,
            'adhesion02': 10.0,
            'adhesion12': 8.0,
            'adhesion22': 4.0,
            'area_deformation_parameter': 20.0,
            'perimeter_deformation_parameter': 15.0,
            'wha_force': 1.0,
            'growth_scaling_parameter': 0.0003,
            'type1_fraction': 0.5}

def run_simulation(simulation, build=False, parallel=False):
    starting_dir = os.path.dirname(os.path.realpath(__file__))
    if build:
        subprocess.call(['scons', 'chaste_libs=1', '-j8'] +
                        (['build=GccOptNative_8'] if parallel else []) +
                        ['test_suite=../test/{sim}.hpp'.format(sim=simulation)])
    else:
        build_dir = '../build/' + ('optimised_native' if parallel else 'debug')
        os.putenv('LD_LIBRARY_PATH', build_dir + ':' + '../../../linklib')
        subprocess.call(['{build_dir}/{sim}Runner'.format(build_dir=build_dir,
                                                           sim=simulation)])
    abs_test_output = os.path.abspath(TESTOUTPUT)
    os.chdir('../../../anim')
    subprocess.call(['java', 'Visualize2dVertexCellsNew',
                     '{test_output}/{sim}/results_from_time_0'.format(test_output=abs_test_output,
                                                                      sim=simulation)])
    os.chdir(starting_dir)
    subprocess.call(['../../../anim/make_a_movie'])
