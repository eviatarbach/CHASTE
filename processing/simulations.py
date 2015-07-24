import os
import subprocess
import ConfigParser
import itertools
import glob

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

def run_simulation(simulation, single_parameters={}, sweep_parameters={}, build=False, optimized=True, animate=False, run=True):
    params = DEFAULTS.copy()
    params.update(single_parameters)
    starting_dir = os.path.abspath('.')
    if build:
        os.chdir('../../..')
        subprocess.call(['scons', 'compile_only=1', 'chaste_libs=1', '-j8'] +
                        (['build=GccOptNative'] if optimized else []) +
                        ['test_suite=projects/{user}/test/{sim}.hpp'.format(user=USER, sim=simulation)])
    os.chdir(starting_dir)

    build_dir = '../build/' + ('optimised_native' if optimized else 'debug')
    os.putenv('LD_LIBRARY_PATH', os.path.abspath(build_dir) + ':' + os.path.abspath('../../../linklib'))

    sim_ids = []
    keys = sweep_parameters.keys()
    values = [sweep_parameters[key] for key in keys]

    if sweep_parameters:
        product = itertools.product(*values)
    else:
        product = [params]

    processes = []
    for param_set in product:
        sweep_params = params.copy()
        if keys:
            for index, value in enumerate(param_set):
                sweep_params[keys[index]] = value
        #TODO: use something better
        sim_id = abs(sum(map(hash, sweep_params.values()))) % 65535
        sim_ids.append(sim_id)
        config = '\n'.join(['{key}={value}'.format(key=key, value=value) for key, value in sweep_params.items()])
        config_file = open('../test/config_{sim_id}'.format(sim_id=sim_id), 'w')
        config_file.write(config)
        config_file.close()

        if run:
            os.chdir(build_dir)
            print("Running simulation {sim_id} with parameters:".format(sim_id=sim_id))
            print(sweep_params)
            proc = subprocess.Popen(['./{sim}Runner'.format(sim=simulation), '-sim_id', '{sim_id}'.format(sim_id=sim_id)])
            os.chdir(starting_dir)
            processes.append(proc)

    # Wait until all processes are finished
    for process in processes:
        process.wait()

    print(sim_ids)
    for sim_id in sim_ids:
        abs_test_output = os.path.abspath(TESTOUTPUT)
        os.chdir('../../../anim')
        subprocess.call(['java', 'Visualize2dVertexCellsNew',
                         '{test_output}/{sim}_{sim_id}/results_from_time_0'.format(test_output=abs_test_output,
                                                                                   sim=simulation,
                                                                                   sim_id=sim_id)])

        if not os.path.isdir(str(sim_id)):
            os.mkdir(str(sim_id))

        print(glob.glob('*.png'))
        for image in glob.glob('*.png'):
            os.rename(image, '{sim_id}/{image}'.format(sim_id=sim_id, image=image))

        os.chdir(starting_dir)

        #os.rename('../test/config_{sim_id}'.format(sim_id=sim_id), '{sim_id}/config_{sim_id}'.format(sim_id=sim_id))

        if animate:
            os.chdir('../../../anim')
            subprocess.call(['./make_a_movie_new', str(sim_id)])
            os.chdir(starting_dir)
