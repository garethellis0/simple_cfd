#ifndef SIMPLE_CFD_FIELD_H
#define SIMPLE_CFD_FIELD_H


class Field {
public:

    /**
     * Construct a new Field from a list of Fields and their distances to this Field
     *
     * @param fields TODO
     */
    virtual Field(std::vector<std::pair<Field, double>> fields) = 0;

};


#endif //SIMPLE_CFD_FIELD_H
