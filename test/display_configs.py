from tabulate import tabulate
import argparse
import os
import json
import re

def main():
    # parse layer to load configs for
    parser = argparse.ArgumentParser()
    parser.add_argument("-p","--path",
            required=True, help="layer/config path")
    args = parser.parse_args()

    # iterate over configs for layer
    configs = {}
    for filepath in os.listdir(os.path.join(args.path, "config")):
        # get config number
        num = int(re.search("([0-9]+)", filepath).group(1))
        # load configs
        with open(os.path.join(args.path, "config", filepath), "r") as f:
            configs[num] = json.load(f)
    print(configs)

    # get the table headers
    headers = ["configuration", "description"]
    config_header = list(configs[0].keys()) # build header of of first config
    config_header.remove("description")
    headers.extend(config_header)

    # create the table
    table = []
    for config in configs:
        row = [config, configs[config]["description"]]
        for header in headers[2:]:
            try:
                row.append(configs[config][header])
            except KeyError:
                print(f"WARNING: configration {config} does not have parameter {header}")
                row.append("-")
        table.append(row)

    # sort the table rows
    table.sort(key=lambda row: row[0])

    # display the table
    print(tabulate(table, headers=headers))

if __name__ == "__main__":
    main()
